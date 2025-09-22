#include "OrbitalSystem.h"

#include "TestHandler.h"
#include "Exception.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

OrbitalSystem::OrbitalSystem(const float hostMass, Length::Absolute const& hostSphereAbsoluteRadius) :
	m_pHostParticle(MakeUnique<HostParticle>(hostMass, hostSphereAbsoluteRadius))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * OrbitalSystem::CreateScalingSphere(ParticleBase * pHostParticle, Length::Absolute const& radius)
{
	const bool isInfluencing = pHostParticle->IsInfluencing() &&
		(radius < pHostParticle->GetSphereOfInfluence()->GetTrueRadius());

	return CreateScalingSphere(pHostParticle, radius, isInfluencing);
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * OrbitalSystem::CreateScalingSphere(ScalingSphereBase * pOuterSphere, Length::Relative const& radius)
{
	return CreateScalingSphere(pOuterSphere->GetHostParticle(), radius.ToAbsolute(*pOuterSphere));
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleBase * OrbitalSystem::CreateParticle(ScalingSphereBase * pHostSpace, float mass, Vector3 const& position, Vector3 const& velocity,
	const bool isInfluencing)
{
	API_ASSERT_THROW(sqrtf(position.SqareMagnitude()) < kScalingSphereEscapeRadius, RESULT_CODE_INVALID_PARAMETER,
		Fmt::Format("Position {} is outside the scaling space!", position));

	if (nullptr != pHostSpace->GetInnerSphere())
	{
		API_ASSERT_THROW(pHostSpace->GetInnerSphere()->GetRadius() < sqrtf(position.SqareMagnitude()), RESULT_CODE_INVALID_PARAMETER,
			Fmt::Format("Position {} is inside the inner scaling space!", position));
	}

	if (isInfluencing)
		return pHostSpace->AddParticle(MakeUnique<InfluencingParticle>(pHostSpace, mass, position, velocity));
	else
		return pHostSpace->AddParticle(MakeUnique<PassiveParticle>(pHostSpace, mass, position, velocity));
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleBase * OrbitalSystem::CreateParticle(ScalingSphereBase * pHostSpace, float mass, Vector3 const& position, const bool isInfluencing)
{
	const float orbitSpeed = pHostSpace->CircularOrbitSpeed(sqrtf(position.SqareMagnitude()));

	assert(false); // TODO - circular orbit velocity direction
	Vector3 circularOrbitVelocity;

	return CreateParticle(pHostSpace, mass, position, circularOrbitVelocity, isInfluencing);
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::DestroyParticle(ParticleBase * pParticleBase)
{
	API_ASSERT_THROW(m_pHostParticle->m_uuid != pParticleBase->m_uuid, RESULT_CODE_INVALID_PARAMETER, "Cannot destroy the host particle");

	ScalingSphereBase * pHostSphere = pParticleBase->GetHostSphere();

	(void) pHostSphere->RemoveParticle(pParticleBase);
}

// --------------------------------------------------------------------------------------------------------------------------------

Result OrbitalSystem::ResizeScalingSphere(ScalingSphereBase * pScalingSphereBase, const float trueRadius)
{
	ParticleBase * pHostParticle = pScalingSphereBase->GetHostParticle();

	return pHostParticle->ResizeScalingSphere(pScalingSphereBase, trueRadius);
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::DestroyScalingSphere(ScalingSphereBase * pScalingSphereBase, const bool shouldDonateParticles)
{
	ParticleBase * pHostParticle = pScalingSphereBase->GetHostParticle();

	(void) pHostParticle->RemoveScalingSphere(pScalingSphereBase, shouldDonateParticles);
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * OrbitalSystem::CreateScalingSphere(ParticleBase * pHostParticle, Length::Absolute const& radius, const bool isInfluencing)
{
	ScalingSphereBase * pNewScalingSphere = nullptr;

	if (isInfluencing)
	{
		pNewScalingSphere = pHostParticle->AddScalingSphere(MakeUnique<InfluencingSphere>(pHostParticle, radius.Get()));

		assert((nullptr == pNewScalingSphere->GetOuterSphere()) || pNewScalingSphere->GetOuterSphere()->IsInfluencing());
	}
	else
	{
		pNewScalingSphere = pHostParticle->AddScalingSphere(MakeUnique<NonInfluencingSpace>(pHostParticle, radius.Get()));

		assert(nullptr != pNewScalingSphere->GetOuterSphere()); // A non-influencing Sphere should always be below an influencing Sphere in the hierarchy.
		assert((pNewScalingSphere->GetOuterSphere()->GetHostParticle() != pNewScalingSphere->GetHostParticle()) ||
			!pNewScalingSphere->GetOuterSphere()->IsInfluencing()); // A non-influencing Sphere cannot be smaller than an influencing Sphere on the same host.
	}

	if ((pNewScalingSphere->GetRadius() < kMinimumScalingSphereRadius) || (kMaximumScalingSphereRadius < pNewScalingSphere->GetRadius()))
	{
		(void) pHostParticle->RemoveScalingSphere(pNewScalingSphere, false);

		return nullptr;
	}

	return pNewScalingSphere;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem::HostParticle::HostParticle(const float mass, const Length::Absolute hostSphereAbsoluteRadius) :
	ParticleBase(nullptr, mass),
	m_pHostSphere(static_cast<InfluencingSphere *>(AddScalingSphere(
		MakeUnique<InfluencingSphere>(static_cast<ParticleBase *>(this), hostSphereAbsoluteRadius.Get()))))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::HostParticle::Initialize()
{
	m_pHostSphere->Initialize();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem::PassiveParticle::PassiveParticle(ScalingSphereBase * pHostSpace, float mass, Vector3 position, Vector3 velocity) :
	ParticleBase(pHostSpace, mass),
	m_position(position),
	m_velocity(velocity),
	m_pOrbit(MakeUnique<Orbit>())
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::PassiveParticle::Initialize()
{
	Orbit::Elements &elements = m_pOrbit->GetCurrentSection().m_elements;

	elements.Compute(GetHostSphere()->GetGravityParameter(), m_position - GetHostSphere()->GetPrimaryPosition(),
		m_velocity - GetHostSphere()->GetPrimaryVelocity());
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem::InfluencingParticle::InfluencingParticle(ScalingSphereBase * pHostSpace, float mass, Vector3 position, Vector3 velocity) :
	PassiveParticle(pHostSpace, mass, position, velocity),
	m_pSphereOfInfluence(nullptr)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::InfluencingParticle::Initialize()
{
	PassiveParticle::Initialize();

	Orbit::Elements &elements = m_pOrbit->GetCurrentSection().m_elements;

	const float radiusOfInfluence = ComputeRadiusOfInfluence(elements.m_semiMajor, GetMass(), GetHostSphere()->GetPrimary()->GetMass());
	const float trueRadiusOfInfluence = radiusOfInfluence * GetHostSphere()->GetTrueRadius();

	if (nullptr == m_pSphereOfInfluence)
	{
		m_pSphereOfInfluence = static_cast<InfluencingSphere *>(AddScalingSphere(
			MakeUnique<InfluencingSphere>(static_cast<ParticleBase *>(this), trueRadiusOfInfluence)));

		m_pSphereOfInfluence->Initialize();
	}
	else
	{
		ResizeScalingSphere(m_pSphereOfInfluence, trueRadiusOfInfluence);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystemTestScript::OrbitalSystemTestScript() :
	ITestScript("OrbitalSystem")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystemTestScript::~OrbitalSystemTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystemTestScript::RunImpl(TestHandler & testHandler)
{
	static constexpr float HOST_MASS = 1e30f;
	static constexpr Length::Absolute HOST_SPACE_RADIUS(8e12f);

	OrbitalSystem orbitalSystem(HOST_MASS, HOST_SPACE_RADIUS);

	ParticleBase & hostParticle = *orbitalSystem.GetHostParticle();

	testHandler.Assert(reinterpret_cast<uintptr_t>(hostParticle.GetHostSphere()), reinterpret_cast<uintptr_t>(nullptr),
		"Host particle's host space is nullptr");

	testHandler.Assert(hostParticle.GetMass(), HOST_MASS, "Host particle mass");
	testHandler.Assert(hostParticle.GetPosition(), Vector3::ZERO, "Host particle position");
	testHandler.Assert(hostParticle.GetVelocity(), Vector3::ZERO, "Host particle velocity");
	testHandler.Assert(hostParticle.IsInfluencing(), true, "Host particle is influencing");

	ScalingSphereBase & hostSpace = *orbitalSystem.GetHostSphere();

	testHandler.Assert(hostSpace.m_uuid, hostParticle.GetScalingSphereList().Front()->m_uuid,
		"Host space is host particle's first attached space");

	testHandler.Assert(hostSpace.GetRadius(), 1.f, "Host space radius");
	testHandler.Assert(hostSpace.GetTrueRadius(), HOST_SPACE_RADIUS.Get(), "Host space true radius");
	testHandler.Assert(hostSpace.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Host space's host particle");
	testHandler.Assert(hostSpace.GetGravityParameter(),
		ScalingSphereBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS.Get(), HOST_MASS), "Host space gravity parameter");
	testHandler.Assert(hostSpace.IsInfluencing(), true, "Host space is influencing");
	testHandler.Assert(hostSpace.GetPrimary()->m_uuid, hostParticle.m_uuid, "Host space's primary");
	testHandler.Assert(hostSpace.GetPrimaryPosition(), Vector3::ZERO, "Host space primary position");
	testHandler.Assert(hostSpace.GetPrimaryVelocity(), Vector3::ZERO, "Host space primary velocity");

	ScalingSphereBase & scaledSpace2 = *orbitalSystem.CreateScalingSphere(&hostParticle, HOST_SPACE_RADIUS / 10.f);

	testHandler.Assert(scaledSpace2.GetRadius(), 0.1f, "Scaled space 2 radius");
	testHandler.Assert(scaledSpace2.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Scaled space 2 host particle");
	testHandler.Assert(scaledSpace2.GetGravityParameter(),
		ScalingSphereBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS.Get() / 10.f, HOST_MASS), "Scaled space 2 gravity parameter");
	testHandler.Assert(scaledSpace2.IsInfluencing(), true, "Scaled space 2 is influencing");
	testHandler.Assert(scaledSpace2.GetPrimary()->m_uuid, hostParticle.m_uuid, "Scaled space 2 primary");
	testHandler.Assert(scaledSpace2.GetPrimaryPosition(), Vector3::ZERO, "Scaled space 2 primary position");
	testHandler.Assert(scaledSpace2.GetPrimaryVelocity(), Vector3::ZERO, "Scaled space 2 primary velocity");

	ScalingSphereBase & scaledSpace3 = *orbitalSystem.CreateScalingSphere(&hostParticle, HOST_SPACE_RADIUS / 100.f);

	testHandler.Assert(scaledSpace3.GetRadius(), 0.1f, "Scaled space 3 radius");
	testHandler.Assert(scaledSpace3.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Scaled space 3 host particle");
	testHandler.Assert(scaledSpace3.GetGravityParameter(),
		ScalingSphereBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS.Get() / 100.f, HOST_MASS), "Scaled space 3 gravity parameter");
	testHandler.Assert(scaledSpace3.IsInfluencing(), true, "Scaled space 3 is influencing");
	testHandler.Assert(scaledSpace3.GetPrimary()->m_uuid, hostParticle.m_uuid, "Scaled space 3 primary");
	testHandler.Assert(scaledSpace3.GetPrimaryPosition(), Vector3::ZERO, "Scaled space 3 primary position");
	testHandler.Assert(scaledSpace3.GetPrimaryVelocity(), Vector3::ZERO, "Scaled space 3 primary velocity");

	testHandler.Assert(orbitalSystem.CreateScalingSphere(&hostParticle, HOST_SPACE_RADIUS * 0.5f) == nullptr, true, "Invalid radius fails ScalingSphere creation");

	/*try
	{
		orbitalSystem.CreateParticle(hostSpace, 1e7f, Vector3(0.05f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), false);
		isException = false;
	}
	catch (ApiException const&)
	{
		isException = true;
	}*/

	testHandler.Assert(orbitalSystem.ResizeScalingSphere(&scaledSpace2, 0.5f * HOST_SPACE_RADIUS.Get()),
		Result(RESULT_CODE_SUCCESS), "ResizeScalingSphere succeeds");

	ScalingSphereBase & smallestSpace = *hostParticle.GetScalingSphereList().Back();

	float const newTrueRadius = 0.5f * smallestSpace.GetTrueRadius();
	float const newRadius = 0.5f * smallestSpace.GetRadius();

	testHandler.Assert(orbitalSystem.ResizeScalingSphere(&smallestSpace, newTrueRadius),
		Result(RESULT_CODE_SUCCESS), "ResizeScalingSphere succeeds");

	testHandler.Assert(smallestSpace.GetRadius(), newRadius, "New radius");
	testHandler.Assert(smallestSpace.GetTrueRadius(), newTrueRadius, "New true radius");
	testHandler.Assert(scaledSpace3.GetGravityParameter(),
		ScalingSphereBase::ComputeScaledGravityParameter(newTrueRadius, HOST_MASS), "New gravity parameter");

	const float particleMass = 1e10f;
	const float orbitRadius = 0.6f;
	const float orbitSpeed = hostSpace.CircularOrbitSpeed(orbitRadius);
	const Vector3 particlePosition(orbitRadius, 0.f, 0.f);
	const Vector3 particleVelocity(0.f, orbitSpeed, 0.f);

	ParticleBase & particle = *orbitalSystem.CreateParticle(&hostSpace, particleMass, particlePosition, particleVelocity, false);

	testHandler.Assert(particle.GetMass(), particleMass, "PassiveParticle mass");
	testHandler.Assert(particle.GetHostSphere()->m_uuid, hostSpace.m_uuid, "Host space");
	testHandler.Assert(particle.GetPosition(), particlePosition, "PassiveParticle position");
	testHandler.Assert(particle.GetVelocity(), particleVelocity, "PassiveParticle velocity");
	testHandler.Assert(particle.IsInfluencing(), false, "Is particle influencing");
	testHandler.Assert(particle.GetScalingSphereList().Size(), 0ull, "PassiveParticle attached spaces");

	const Length::Relative particleScaledSpaceRadius(0.05f);
	const Length::Absolute particleScaledSpaceAbsoluteRadius = particleScaledSpaceRadius.ToAbsolute(hostSpace);

	ScalingSphereBase & particleScaledSpace = *orbitalSystem.CreateScalingSphere(&particle, particleScaledSpaceAbsoluteRadius);

	testHandler.Assert(particleScaledSpace.GetHostParticle()->m_uuid, particle.m_uuid, "PassiveParticle scaled space host particle");
	testHandler.Assert(particleScaledSpace.GetParticleList().size(), 0ull, "PassiveParticle scaled space particle list size");
	testHandler.Assert(particleScaledSpace.GetOuterSphere()->m_uuid, hostSpace.m_uuid, "PassiveParticle scaled space outer space");
	testHandler.Assert(reinterpret_cast<uintptr_t>(particleScaledSpace.GetInnerSphere()), reinterpret_cast<uintptr_t>(nullptr), "PassiveParticle scaled space inner space");

	testHandler.Assert(particleScaledSpace.GetTrueRadius(), particleScaledSpaceAbsoluteRadius.Get(), "PassiveParticle scaled space true radius");
	testHandler.Assert(particleScaledSpace.GetRadius(), particleScaledSpaceRadius.Get(), "PassiveParticle scaled space radius");

	const float expectedGravityParameter = ScalingSphereBase::ComputeScaledGravityParameter(particleScaledSpaceAbsoluteRadius.Get(), HOST_MASS);
	testHandler.Assert(particleScaledSpace.GetGravityParameter(), expectedGravityParameter, "PassiveParticle scaled space gravity parameter");

	testHandler.Assert(particleScaledSpace.IsInfluencing(), false, "PassiveParticle scaled space is influencing");
	testHandler.Assert(particleScaledSpace.GetPrimary()->m_uuid, hostParticle.m_uuid, "PassiveParticle scaled space primary");
	testHandler.Assert(sqrtf(particleScaledSpace.GetPrimaryPosition().SqareMagnitude()), orbitRadius / particleScaledSpaceRadius.Get(), "PassiveParticle scaled space primary distance");
	testHandler.Assert(particleScaledSpace.GetPrimaryVelocity(), particleVelocity * -1.f / particleScaledSpaceRadius.Get(), "PassiveParticle scaled space primary velocity");

	const float particleScaledSpaceNewRadius = 0.04f;
	const float particleScaledSpaceNewTrueRadius = particle.GetHostSphere()->GetTrueRadius() * particleScaledSpaceNewRadius;

	testHandler.Assert(orbitalSystem.ResizeScalingSphere(&particleScaledSpace, particleScaledSpaceNewTrueRadius),
		Result(RESULT_CODE_SUCCESS), "ResizeScalingSphere succeeds");

	testHandler.Assert(particleScaledSpace.GetTrueRadius(), particleScaledSpaceNewTrueRadius, "PassiveParticle scaled space new true radius");
	testHandler.Assert(particleScaledSpace.GetRadius(), particleScaledSpaceNewRadius, "PassiveParticle scaled space new radius");
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereListTestScript::ScalingSphereListTestScript() :
	ITestScript("ScalingSphereList")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereListTestScript::~ScalingSphereListTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereListTestScript::RunImpl(TestHandler & testHandler)
{
	using InfluencingSphere = OrbitalSystem::InfluencingSphere;

	static constexpr float TRUE_RADIUS_1 = 100.f;
	static constexpr float TRUE_RADIUS_2 = 80.f;
	static constexpr float TRUE_RADIUS_3 = 90.f;

	ScalingSphereList list;
	ScalingSphereList::Iterator insertedIterator;

	testHandler.Assert(list.Empty(), true, "Newly constructed list is empty");

	UniquePtr<ParticleBase> particlePtr = MakeUnique<OrbitalSystem::PassiveParticle>(nullptr, 1.f, Vector3::ZERO, Vector3::ZERO);

	UniquePtr<ScalingSphereBase> scalingSpherePtr = MakeUnique<InfluencingSphere>(particlePtr.get(), TRUE_RADIUS_1);
	const uint64_t uuid1 = scalingSpherePtr->m_uuid.Get();

	testHandler.Assert(nullptr == scalingSpherePtr, false, "ScalingSpherePtr is not null before insertion into list");

	{
		insertedIterator = list.Insert(std::move(scalingSpherePtr));

		(*insertedIterator)->Initialize();

		testHandler.Assert(nullptr == scalingSpherePtr.get(), true, "ScalingSpherePtr is null after insertion into list");

		testHandler.Assert(list.Empty(), false, "List is not empty after insertion");
		testHandler.Assert(reinterpret_cast<uintptr_t>(list.Begin()->get()), reinterpret_cast<uintptr_t>(insertedIterator->get()),
			"Begin iterator is the new inserted iterator");
		testHandler.Assert(list.Front()->GetOuterSphere() == nullptr, true, "Lone element outer Sphere is null");
		testHandler.Assert(list.Front()->GetInnerSphere() == nullptr, true, "Lone element inner Sphere is null");

		testHandler.Assert(list.Front()->m_uuid.Get(), uuid1, "Inserted element UUID equals the constructed ScalingSphere's UUID");
		testHandler.Assert(list.Front()->GetTrueRadius(), TRUE_RADIUS_1, "Inserted element TrueRadius");
	}

	scalingSpherePtr = MakeUnique<InfluencingSphere>(particlePtr.get(), TRUE_RADIUS_2);
	const uint64_t uuid2 = scalingSpherePtr->m_uuid.Get();

	{
		insertedIterator = list.Insert(std::move(scalingSpherePtr));

		(*insertedIterator)->Initialize();

		testHandler.Assert(list.Size(), 2ull, "Size of list after second insertion");
		testHandler.Assert(reinterpret_cast<uintptr_t>((++list.Begin())->get()), reinterpret_cast<uintptr_t>(insertedIterator->get()),
			"Begin iterator is the new inserted iterator");
		testHandler.Assert(list.Front()->GetOuterSphere() == nullptr, true, "First element outer Sphere is null");
		testHandler.Assert(list.Back()->GetInnerSphere() == nullptr, true, "Last element inner Sphere is null");
		testHandler.Assert((list.Front()->GetInnerSphere() == list.Back().get()) && (list.Front().get() == list.Back()->GetOuterSphere()),
			true, "Inner-outer Spheres point to each other");

		testHandler.Assert(list.Back()->m_uuid.Get(), uuid2, "Inserted ScalingSphere UUID equals the constructed ScalingSphere's UUID");
		testHandler.Assert(list.Back()->GetTrueRadius(), TRUE_RADIUS_2, "Inserted element TrueRadius");
		testHandler.Assert(list.Back()->GetRadius(), TRUE_RADIUS_2 / TRUE_RADIUS_1, "Inserted element Radius");
	}

	scalingSpherePtr = MakeUnique<InfluencingSphere>(particlePtr.get(), TRUE_RADIUS_3);
	const uint64_t uuid3 = scalingSpherePtr->m_uuid.Get();

	{
		insertedIterator = list.Insert(std::move(scalingSpherePtr));

		ScalingSphereBase * pNewScalingSphere = insertedIterator->get();

		pNewScalingSphere->Initialize();
		list.Back()->Initialize();

		testHandler.Assert(list.Size(), 3ull, "Size of list after third insertion");
		testHandler.Assert(reinterpret_cast<uintptr_t>((++list.Begin())->get()), reinterpret_cast<uintptr_t>(insertedIterator->get()),
			"Begin iterator is the new inserted iterator");

		testHandler.Assert(list.Front()->GetOuterSphere() == nullptr, true, "First element outer Sphere is null");
		testHandler.Assert(list.Back()->GetInnerSphere() == nullptr, true, "Last element inner Sphere is null");
		testHandler.Assert((list.Front()->GetInnerSphere() == pNewScalingSphere) && (pNewScalingSphere == list.Back()->GetOuterSphere()),
			true, "Inner-outer Spheres point to new middle element");
		testHandler.Assert((list.Front().get() == pNewScalingSphere->GetOuterSphere()) && (pNewScalingSphere->GetInnerSphere() == list.Back().get()),
			true, "Middle element points to inner-outer Spheres");

		testHandler.Assert(pNewScalingSphere->GetTrueRadius(), TRUE_RADIUS_3, "Inserted element TrueRadius");
		testHandler.Assert(pNewScalingSphere->GetRadius(), TRUE_RADIUS_3 / TRUE_RADIUS_1, "Inserted element Radius");
		testHandler.Assert(list.Back()->GetRadius(), TRUE_RADIUS_2 / TRUE_RADIUS_3, "Last element new Radius");
	}

	scalingSpherePtr = list.Remove(insertedIterator);
	{
		list.Back()->Initialize();

		testHandler.Assert(nullptr == scalingSpherePtr, false, "ScalingSpherePtr is not null after removal from list");
		testHandler.Assert(scalingSpherePtr->m_uuid.Get(), uuid3, "Removed ScalingSphere UUID equals the constructed ScalingSphere's UUID");
		testHandler.Assert(scalingSpherePtr->GetTrueRadius(), TRUE_RADIUS_3, "Removed ScalingSphere TrueRadius");

		testHandler.Assert(list.Size(), 2ull, "Size of list after removing middle element");
		testHandler.Assert(list.Front()->GetTrueRadius() > list.Back()->GetTrueRadius(), true, "Ordering of elements preserved after removal");
		testHandler.Assert(list.Back()->GetRadius(), TRUE_RADIUS_2 / TRUE_RADIUS_1, "Last element new Radius");
		testHandler.Assert((list.Front()->GetInnerSphere() == list.Back().get()) && (list.Front().get() == list.Back()->GetOuterSphere()),
			true, "Inner-outer Spheres point to each other");
	}

	scalingSpherePtr = list.Remove(--list.End());
	{
		list.Front()->Initialize();

		testHandler.Assert(nullptr == scalingSpherePtr, false, "ScalingSpherePtr is not null after removal from list");
		testHandler.Assert(scalingSpherePtr->m_uuid.Get(), uuid2, "Removed ScalingSphere UUID equals the constructed ScalingSphere's UUID");
		testHandler.Assert(scalingSpherePtr->GetTrueRadius(), TRUE_RADIUS_2, "Removed ScalingSphere TrueRadius");

		testHandler.Assert(list.Size(), 1ull, "Size of list after removing last element");
		testHandler.Assert(list.Front()->m_uuid.Get(), list.Back()->m_uuid.Get(), "First element is now also the last element");
	}
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

ResizeScalingSpheresTestScript::ResizeScalingSpheresTestScript() :
	ITestScript("Resizing ScalingSpheres")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ResizeScalingSpheresTestScript::~ResizeScalingSpheresTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void ResizeScalingSpheresTestScript::RunImpl(TestHandler & testHandler)
{
	using namespace Length;

	static constexpr float HOST_MASS = 1e10f;
	static constexpr Absolute HOST_SPHERE_ABSOLUTE_RADIUS(1000.f);

	static constexpr Relative P0_ORBIT_RADIUS(0.5f);
	static constexpr RelVector3 P0_POSITION = { P0_ORBIT_RADIUS, Relative(0.f), Relative(0.f) };
	static constexpr float P0_MASS = 1e5f;

	static constexpr Absolute S1_ABSOLUTE_RADIUS(200.f);
	static constexpr Absolute S2_ABSOLUTE_RADIUS(40.f);
	static constexpr Absolute S3_ABSOLUTE_RADIUS(8.f);

	static constexpr Relative P1_ORBIT_RADIUS(0.9f);
	static constexpr Relative P2_ORBIT_RADIUS(0.5f);
	static constexpr Relative P3_ORBIT_RADIUS(0.5f);
	static constexpr RelVector3 P1_POSITION = { P1_ORBIT_RADIUS, Relative(0.f), Relative(0.f) };
	static constexpr RelVector3 P2_POSITION = { P2_ORBIT_RADIUS, Relative(0.f), Relative(0.f) };
	static constexpr RelVector3 P3_POSITION = { P3_ORBIT_RADIUS, Relative(0.f), Relative(0.f) };
	static constexpr float P1_MASS = 1.f;
	static constexpr float P2_MASS = 1.f;
	static constexpr float P3_MASS = 1.f;

	OrbitalSystem orbitalSystem(HOST_MASS, HOST_SPHERE_ABSOLUTE_RADIUS);

	static const Relative S1_RELATIVE_RADIUS(S1_ABSOLUTE_RADIUS, *orbitalSystem.GetHostSphere());
	static const Relative S2_RELATIVE_RADIUS(S2_ABSOLUTE_RADIUS, *orbitalSystem.GetHostSphere());
	static const Relative S3_RELATIVE_RADIUS(S3_ABSOLUTE_RADIUS, *orbitalSystem.GetHostSphere());

	const RelVector3 P0_VELOCITY = { 0.f, orbitalSystem.GetHostSphere()->CircularOrbitSpeed(P0_ORBIT_RADIUS), 0.f };

	ParticleBase *const pP0 = orbitalSystem.CreateParticle(orbitalSystem.GetHostSphere(), P0_MASS, P0_POSITION, Vector3::Y1, false);
	assert(nullptr != pP0);

	ScalingSphereBase *const pS1 = orbitalSystem.CreateScalingSphere(pP0, S1_ABSOLUTE_RADIUS);
	ScalingSphereBase *const pS2 = orbitalSystem.CreateScalingSphere(pP0, S2_ABSOLUTE_RADIUS);
	ScalingSphereBase *const pS3 = orbitalSystem.CreateScalingSphere(pP0, S3_ABSOLUTE_RADIUS);
	assert(nullptr != pS1);
	assert(nullptr != pS2);
	assert(nullptr != pS3);

	testHandler.Assert(pS1->GetRadius(), S1_RELATIVE_RADIUS.Get(), "S1 relative radius");
	testHandler.Assert(pS2->GetRadius(), S2_RELATIVE_RADIUS.Get(), "S2 relative radius");
	testHandler.Assert(pS3->GetRadius(), S3_RELATIVE_RADIUS.Get(), "S3 relative radius");

	const Vector3 P1_VELOCITY = { 0.f, pS1->CircularOrbitSpeed(P1_ORBIT_RADIUS), 0.f };
	const Vector3 P2_VELOCITY = { 0.f, pS2->CircularOrbitSpeed(P2_ORBIT_RADIUS), 0.f };
	const Vector3 P3_VELOCITY = { 0.f, pS3->CircularOrbitSpeed(P3_ORBIT_RADIUS), 0.f };

	ParticleBase *const pP1 = orbitalSystem.CreateParticle(pS1, P1_MASS, P1_POSITION, P1_VELOCITY, false);
	ParticleBase *const pP2 = orbitalSystem.CreateParticle(pS2, P2_MASS, P2_POSITION, P2_VELOCITY, false);
	ParticleBase *const pP3 = orbitalSystem.CreateParticle(pS3, P3_MASS, P3_POSITION, P3_VELOCITY, false);
	assert(nullptr != pP1);
	assert(nullptr != pP2);
	assert(nullptr != pP3);

	testHandler.Assert(static_cast<int>(pP1->GetOrbit()->GetCurrentSection().m_elements.m_type), static_cast<int>(Orbit::Type::Circle), "P1 orbit is circular");
	testHandler.Assert(static_cast<int>(pP2->GetOrbit()->GetCurrentSection().m_elements.m_type), static_cast<int>(Orbit::Type::Circle), "P2 orbit is circular");
	testHandler.Assert(static_cast<int>(pP3->GetOrbit()->GetCurrentSection().m_elements.m_type), static_cast<int>(Orbit::Type::Circle), "P3 orbit is circular");

	// Resize S2 to 10.
	testHandler.Assert<Result, bool>([&](bool)
	{
		static const Length::Absolute S2_NEW_ABSOLUTE_RADIUS(10.f);
		static const Length::Relative S2_NEW_RELATIVE_RADIUS(S2_NEW_ABSOLUTE_RADIUS, *pS1);
		static const Vector3 P2_NEW_POSITION = P2_POSITION * S2_RELATIVE_RADIUS;

		const Result result = pP0->ResizeScalingSphere(pS2, S2_NEW_ABSOLUTE_RADIUS);

		testHandler.Assert(pS2->GetRadius(), S2_NEW_RELATIVE_RADIUS.Get(), "S2 new relative radius");
		testHandler.Assert((pS2->GetOuterSphere() == pS1) && (pS2->GetInnerSphere() == pS3), true, "S2 ordering maintained");

		testHandler.Assert(pP2->GetHostSphere() == pS1, true, "P2 moved to S1");
		testHandler.Assert(pP2->GetPosition(), P2_NEW_POSITION, "P2 new relative position");
		testHandler.Assert(pP2->GetPosition() * pS1->GetTrueRadius(), P2_POSITION * S2_ABSOLUTE_RADIUS, "P2 absolute position maintained");

		return result;
	}, true, Result(RESULT_CODE_SUCCESS), "Resize S2 to 10");

	// Resize S2 back to 40.
	testHandler.Assert<Result, bool>([&](bool)
	{
		const Result result = pP0->ResizeScalingSphere(pS2, S2_ABSOLUTE_RADIUS);

		testHandler.Assert(pS2->GetRadius(), S2_RELATIVE_RADIUS.Get(), "S2 relative radius restored");
		testHandler.Assert((pS2->GetOuterSphere() == pS1) && (pS2->GetInnerSphere() == pS3), true, "S2 ordering maintained");

		testHandler.Assert(pP2->GetHostSphere() == pS2, true, "P2 moved back to S2");
		testHandler.Assert(pP2->GetPosition(), P2_POSITION, "P2 relative position restored");
		testHandler.Assert(pP2->GetPosition() * pS2->GetTrueRadius(), P2_POSITION * S2_ABSOLUTE_RADIUS, "P2 absolute position maintained");

		return result;
	}, true, Result(RESULT_CODE_SUCCESS), "Resize S2 back to 40");

	// Resize S2 to 6.
	testHandler.Assert<Result, bool>([&](bool)
	{
		static const Length::Absolute S2_NEW_ABSOLUTE_RADIUS(6.f);
		static const Length::Relative S2_NEW_RELATIVE_RADIUS(S2_NEW_ABSOLUTE_RADIUS / S3_ABSOLUTE_RADIUS);
		static const Length::Relative S3_NEW_RELATIVE_RADIUS(S3_ABSOLUTE_RADIUS / S1_ABSOLUTE_RADIUS);
		static const Vector3 P2_NEW_POSITION = P2_POSITION * S2_RELATIVE_RADIUS;
		static const Vector3 P3_NEW_POSITION = P3_POSITION * S3_ABSOLUTE_RADIUS / S2_NEW_ABSOLUTE_RADIUS;

		const Result result = pP0->ResizeScalingSphere(pS2, S2_NEW_ABSOLUTE_RADIUS);

		testHandler.Assert(pS2->GetRadius(), S2_NEW_RELATIVE_RADIUS.Get(), "S2 new relative radius");
		testHandler.Assert((pS2->GetOuterSphere() == pS3) && (pS2->GetInnerSphere() == nullptr), true, "S2 ordering changed");

		testHandler.Assert((pS3->GetOuterSphere() == pS1) && (pS3->GetInnerSphere() == pS2), true, "S3 ordering changed");
		testHandler.Assert(pS3->GetRadius(), S3_NEW_RELATIVE_RADIUS.Get(), "S3 new relative radius");

		testHandler.Assert((pS1->GetOuterSphere() == orbitalSystem.GetHostSphere()) && (pS1->GetInnerSphere() == pS3), true, "S1 ordering maintained");

		testHandler.Assert(pP2->GetHostSphere() == pS1, true, "P2 moved to S1");
		testHandler.Assert(pP2->GetPosition(), P2_NEW_POSITION, "P2 new relative position");
		testHandler.Assert(pP2->GetPosition() * pS1->GetTrueRadius(), P2_POSITION * S2_ABSOLUTE_RADIUS, "P2 absolute position maintained");

		testHandler.Assert(pP3->GetHostSphere() == pS2, true, "P3 moved to S2");
		testHandler.Assert(pP3->GetPosition(), P3_NEW_POSITION, "P3 new relative position");
		testHandler.Assert(pP3->GetPosition() * S2_NEW_ABSOLUTE_RADIUS, P3_POSITION * S3_ABSOLUTE_RADIUS, "P3 absolute position maintained");

		return result;
	}, true, Result(RESULT_CODE_SUCCESS), "Resize S2 to 6");

	// Resize S2 back to 40.
	testHandler.Assert<Result, bool>([&](bool)
	{
		const Result result = pP0->ResizeScalingSphere(pS2, S2_ABSOLUTE_RADIUS);

		testHandler.Assert(pS2->GetRadius(), S2_RELATIVE_RADIUS.Get(), "S2 relative radius restored");
		testHandler.Assert((pS2->GetOuterSphere() == pS1) && (pS2->GetInnerSphere() == pS3), true, "S2 ordering restored");

		testHandler.Assert(pP2->GetHostSphere() == pS2, true, "P2 moved back to S2");
		testHandler.Assert(pP2->GetPosition(), P2_POSITION, "P2 relative position restored");
		testHandler.Assert(pP2->GetPosition() * pS2->GetTrueRadius(), P2_POSITION * S2_ABSOLUTE_RADIUS, "P2 absolute position maintained");

		testHandler.Assert(pP3->GetHostSphere() == pS3, true, "P3 moved back to S3");
		testHandler.Assert(pP3->GetPosition(), P3_POSITION, "P3 relative position");
		testHandler.Assert(pP3->GetPosition() * pS3->GetTrueRadius(), P3_POSITION * S3_ABSOLUTE_RADIUS, "P3 absolute position maintained");

		return result;
	}, true, Result(RESULT_CODE_SUCCESS), "Resize S2 back to 50");

	// Resize S1 to 160.
	testHandler.Assert<Result, bool>([&](bool)
	{
		static const Length::Absolute S1_NEW_ABSOLUTE_RADIUS(160.f);
		static const Length::Relative S1_NEW_RELATIVE_RADIUS(S1_NEW_ABSOLUTE_RADIUS / HOST_SPHERE_ABSOLUTE_RADIUS);
		static const Length::Relative S2_NEW_RELATIVE_RADIUS(S2_ABSOLUTE_RADIUS / S1_NEW_ABSOLUTE_RADIUS);
		static const Vector3 P1_NEW_POSITION = P0_POSITION + (P1_POSITION * S1_RELATIVE_RADIUS);

		const Result result = pP0->ResizeScalingSphere(pS1, S1_NEW_ABSOLUTE_RADIUS);

		testHandler.Assert(pS1->GetRadius(), S1_NEW_RELATIVE_RADIUS.Get(), "S1 new relative radius");
		testHandler.Assert((pS1->GetOuterSphere() == orbitalSystem.GetHostSphere()) && (pS1->GetInnerSphere() == pS2), true, "S1 ordering maintained");

		testHandler.Assert(pP1->GetHostSphere() == orbitalSystem.GetHostSphere(), true, "P1 moved to host Sphere");
		testHandler.Assert(pP1->GetPosition(), P1_NEW_POSITION, "P1 new relative position");
		testHandler.Assert(pP1->GetPosition() * HOST_SPHERE_ABSOLUTE_RADIUS, (P0_POSITION * HOST_SPHERE_ABSOLUTE_RADIUS) + (P1_POSITION * S1_ABSOLUTE_RADIUS), "P1 absolute position maintained");

		return result;
	}, true, Result(RESULT_CODE_SUCCESS), "Resize S1 to 160");

	// Resize S1 back to 200.
	testHandler.Assert<Result, bool>([&](bool)
	{
		const Result result = pP0->ResizeScalingSphere(pS1, S1_ABSOLUTE_RADIUS);

		testHandler.Assert(pS1->GetRadius(), S1_RELATIVE_RADIUS.Get(), "S1 relative radius restored");
		testHandler.Assert((pS1->GetOuterSphere() == orbitalSystem.GetHostSphere()) && (pS1->GetInnerSphere() == pS2), true, "S1 ordering maintained");

		testHandler.Assert(pP1->GetHostSphere() == pS1, true, "P1 moved back to S1");
		testHandler.Assert(pP1->GetPosition(), P1_POSITION, "P1 relative position restored");
		testHandler.Assert(pP1->GetPosition() * pS1->GetTrueRadius(), P1_POSITION * S1_ABSOLUTE_RADIUS, "P1 absolute position maintained");

		return result;
	}, true, Result(RESULT_CODE_SUCCESS), "Resize S1 back to 100");

	// Remove S3.
	testHandler.Assert<bool, bool>([&](bool)
	{
		static const Vector3 P3_NEW_POSITION = P3_POSITION * S3_RELATIVE_RADIUS;

		UniquePtr<ScalingSphereBase> s3ptr = pP0->RemoveScalingSphere(pS3, true);

		testHandler.Assert(s3ptr != nullptr, true, "Removed S3 pointer is not null");
		testHandler.Assert(s3ptr->GetParticleList().size(), 0ull, "Particle list size is zero");

		testHandler.Assert((pS2->GetOuterSphere() == pS1) && (pS2->GetInnerSphere() == nullptr), true, "S2 ordering updated");

		testHandler.Assert(pP3->GetHostSphere() == pS2, true, "P3 moved to S2");
		testHandler.Assert(pP3->GetPosition(), P3_NEW_POSITION, "P3 new relative position");
		testHandler.Assert(pP3->GetPosition() * S2_ABSOLUTE_RADIUS, P3_POSITION * S3_ABSOLUTE_RADIUS, "P3 absolute position maintained");

		return true;
	}, true, true, "Remove S3");

	// Remove S1.
	testHandler.Assert<bool, bool>([&](bool)
	{
		static const Vector3 P1_NEW_POSITION = P0_POSITION + (P1_POSITION * S1_RELATIVE_RADIUS);

		UniquePtr<ScalingSphereBase> s1ptr = pP0->RemoveScalingSphere(pS1, true);

		testHandler.Assert(s1ptr != nullptr, true, "Removed S1 pointer is not null");
		testHandler.Assert(s1ptr->GetParticleList().size(), 0ull, "Particle list size is zero");

		testHandler.Assert((pS2->GetOuterSphere() == orbitalSystem.GetHostSphere()) && (pS2->GetInnerSphere() == nullptr), true, "S2 ordering updated");

		testHandler.Assert(pP1->GetHostSphere() == orbitalSystem.GetHostSphere(), true, "P1 moved to host Sphere");
		testHandler.Assert(pP1->GetPosition(), P1_NEW_POSITION, "P1 new relative position");
		testHandler.Assert(pP1->GetPosition() * HOST_SPHERE_ABSOLUTE_RADIUS, P0_POSITION + (P1_POSITION * S1_ABSOLUTE_RADIUS), "P1 absolute position maintained");

		return true;
	}, true, true, "Remove S1");
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

RelAbsVectorTestScript::RelAbsVectorTestScript() :
	ITestScript("Rel/AbsVector")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

RelAbsVectorTestScript::~RelAbsVectorTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void RelAbsVectorTestScript::RunImpl(TestHandler & testHandler)
{
	const AbsVector3 absZero = AbsVector3::ZERO;
	const AbsVector3 absX1 = AbsVector3::X1;
	const AbsVector3 absY1 = AbsVector3::Y1;
	const AbsVector3 absZ1 = AbsVector3::Z1;

	const AbsVector3 absV1 = absZero + absX1 + absY1 + absZ1;

	const RelVector3 relZero = RelVector3::ZERO;
	const RelVector3 relX1 = RelVector3::X1;
	const RelVector3 relY1 = RelVector3::Y1;
	const RelVector3 relZ1 = RelVector3::Z1;

	const RelVector3 relV1 = relZero + relX1 + relY1 + relZ1;

	const AbsVector3 absV2 = relV1; // Correctly fails to compile due to explicit constructor.
	const RelVector3 relV2 = absV1; // Correctly fails to compile due to explicit constructor.
	const AbsVector3 absV2 = absV1 + relV1; // Correctly fails to compile due to explicit constructor.
	const RelVector3 relV2 = relV1 + absV1; // Correctly fails to compile due to explicit constructor.

	const RelVector3 relV2 = RelVector3(
		Length::Relative(absV1.X().Get()),
		Length::Relative(absV1.Y().Get()),
		Length::Relative(absV1.Z().Get()));

	RelVector3 relV3;
	AbsVector3 absV3;

	relV3 = relV3 + absV3;
	absV3 = relV3 + absV3;
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
