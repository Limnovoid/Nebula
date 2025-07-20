#include "OrbitalSystem.h"

#include "TestHandler.h"
#include "Exception.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

OrbitalSystem::OrbitalSystem(float hostMass, float hostSpaceTrueRadius) :
	m_pHostParticle(MakeUnique<HostParticle>(hostMass))
{
	ScalingSphereBase * pHostSpace = m_pHostParticle->AddScalingSphere(MakeUnique<InfluencingSpace>(m_pHostParticle.get(), hostSpaceTrueRadius));
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * OrbitalSystem::CreateScaledSpace(ParticleBase & hostParticle, float trueRadius)
{
	bool isInfluencing = false;

	if (!hostParticle.GetScalingSphereList().Empty())
	{
		if (trueRadius < hostParticle.GetScalingSphereList().Back()->m_trueRadius)
		{
			isInfluencing = hostParticle.IsInfluencing();
		}
		else if (trueRadius < hostParticle.GetScalingSphereList().Front()->m_trueRadius)
		{
			throw ApiException(RESULT_CODE_INVALID_PARAMETER,
				"True radius must be greater or less than all existing scaled spaces on the particle.");
		}
	}

	return CreateScaledSpaceImpl(&hostParticle, trueRadius, isInfluencing);
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleBase * OrbitalSystem::CreateParticle(ScalingSphereBase & hostSpace, float mass, Vector3 position, Vector3 velocity,
	bool isInfluencing)
{
	API_ASSERT_THROW(sqrtf(position.SqareMagnitude()) < kScalingSpaceEscapeRadius, RESULT_CODE_INVALID_PARAMETER,
		Fmt::Format("Position {} is outside the scaling space!", position));

	if (nullptr != hostSpace.m_pInnerSpace)
	{
		API_ASSERT_THROW(hostSpace.m_pInnerSpace->m_radius < sqrtf(position.SqareMagnitude()), RESULT_CODE_INVALID_PARAMETER,
			Fmt::Format("Position {} is inside the inner scaling space!", position));
	}

	UniquePtr<ParticleBase> &pNewParticle = hostSpace.m_particles.emplace_back();

	if (isInfluencing)
		pNewParticle = MakeUnique<InfluencingParticle>(&hostSpace, mass, position, velocity);
	else
		pNewParticle = MakeUnique<PassiveParticle>(&hostSpace, mass, position, velocity);

	return pNewParticle.get();
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleBase * OrbitalSystem::CreateParticle(ScalingSphereBase & hostSpace, float mass, Vector3 position, bool isInfluencing)
{
	const float orbitSpeed = hostSpace.CircularOrbitSpeed(sqrtf(position.SqareMagnitude()));

	assert(false); // TODO - circular orbit velocity direction
	Vector3 circularOrbitVelocity;

	return CreateParticle(hostSpace, mass, position, circularOrbitVelocity, isInfluencing);
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::DestroyParticle(ParticleBase * pParticleBase)
{
	API_ASSERT_THROW(m_pHostParticle->m_uuid != pParticleBase->m_uuid, RESULT_CODE_INVALID_PARAMETER, "Cannot destroy the host particle");

	ScalingSphereBase::ParticleList & particleList = pParticleBase->GetHostSphere()->m_particles;

	for (ScalingSphereBase::ParticleList::const_iterator citerator = particleList.cbegin(); particleList.cend() != citerator; ++citerator)
	{
		if ((*citerator)->m_uuid == pParticleBase->m_uuid)
		{
			particleList.erase(citerator);
			return;
		}
	}

	assert(false); // We shouldn't be here!
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * OrbitalSystem::CreateScaledSpaceImpl(ParticleBase * pHostParticle, float trueRadius, bool isInfluencing)
{
	ScalingSphereBase * pNewScalingSphere = nullptr;

	if (isInfluencing)
	{
		pNewScalingSphere = pHostParticle->AddScalingSphere(MakeUnique<InfluencingSpace>(pHostParticle, trueRadius));

		assert((nullptr == pNewScalingSphere->m_pOuterSpace) || pNewScalingSphere->m_pOuterSpace->IsInfluencing());
	}
	else
	{
		pNewScalingSphere = pHostParticle->AddScalingSphere(MakeUnique<NonInfluencingSpace>(pHostParticle, trueRadius));

		assert(nullptr != pNewScalingSphere->m_pOuterSpace); // A non-influencing sphere should always be contained in a large sphere.
		assert(!pNewScalingSphere->m_pOuterSpace->IsInfluencing() || // A non-influencing sphere cannot be smaller than an influencing sphere on the same host.
			(pNewScalingSphere->m_pOuterSpace->m_pHostParticle != pNewScalingSphere->m_pHostParticle));
	}

	if (!((kMinimumScalingSpaceRadius <= pNewScalingSphere->m_radius) && (pNewScalingSphere->m_radius < kMaximumScalingSpaceRadius)))
	{
		(void) pHostParticle->RemoveScalingSphere(pNewScalingSphere);

		API_ASSERT_THROW(false, RESULT_CODE_INVALID_PARAMETER,
			Fmt::Format("Radius must be a value in the range [{}, {})", kMinimumScalingSpaceRadius, kMaximumScalingSpaceRadius));
	}

	return pNewScalingSphere;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem::HostParticle::HostParticle(float hostMass) :
	ParticleBase(nullptr, hostMass)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::HostParticle::Initialize()
{
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

	elements.Compute(m_pHostSphere->GetGravityParameter(), m_position - m_pHostSphere->GetPrimaryPosition(),
		m_velocity - m_pHostSphere->GetPrimaryVelocity());
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem::InfluencingParticle::InfluencingParticle(ScalingSphereBase * pHostSpace, float mass, Vector3 position, Vector3 velocity) :
	PassiveParticle(pHostSpace, mass, position, velocity)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem::InfluencingParticle::Initialize()
{
	PassiveParticle::Initialize();

	Orbit::Elements &elements = m_pOrbit->GetCurrentSection().m_elements;

	const float radiusOfInfluence = ComputeRadiusOfInfluence(elements.m_semiMajor, m_mass, m_pHostSphere->GetPrimary()->GetMass());
	const float trueRadiusOfInfluence = radiusOfInfluence * m_pHostSphere->GetTrueRadius();

	if (nullptr == m_pSphereOfInfluence)
	{
		m_pSphereOfInfluence = static_cast<InfluencingSpace *>(AddScalingSphere(
			MakeUnique<InfluencingSpace>(static_cast<ParticleBase *>(this), trueRadiusOfInfluence)));

		m_pSphereOfInfluence->Initialize();
	}
	else
	{
		assert(false); // TODO - resize SphereOfInfluence...
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
	static constexpr float HOST_SPACE_RADIUS = 8e12f;

	OrbitalSystem orbitalSystem(HOST_MASS, HOST_SPACE_RADIUS);

	ParticleBase & hostParticle = *orbitalSystem.GetHostParticle();

	testHandler.Assert(reinterpret_cast<uintptr_t>(hostParticle.GetHostSphere()), reinterpret_cast<uintptr_t>(nullptr),
		"Host particle's host space is nullptr");

	testHandler.Assert(hostParticle.GetMass(), HOST_MASS, "Host particle mass");
	testHandler.Assert(hostParticle.GetPosition(), Vector3::Zero(), "Host particle position");
	testHandler.Assert(hostParticle.GetVelocity(), Vector3::Zero(), "Host particle velocity");
	testHandler.Assert(hostParticle.IsInfluencing(), true, "Host particle is influencing");

	ScalingSphereBase & hostSpace = *orbitalSystem.GetHostSpace();

	testHandler.Assert(hostSpace.m_uuid, hostParticle.GetScalingSphereList().Front()->m_uuid,
		"Host space is host particle's first attached space");

	testHandler.Assert(hostSpace.GetRadius(), 1.f, "Host space radius");
	testHandler.Assert(hostSpace.GetTrueRadius(), HOST_SPACE_RADIUS, "Host space true radius");
	testHandler.Assert(hostSpace.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Host space's host particle");
	testHandler.Assert(hostSpace.GetGravityParameter(),
		ScalingSphereBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS, HOST_MASS), "Host space gravity parameter");
	testHandler.Assert(hostSpace.IsInfluencing(), true, "Host space is influencing");
	testHandler.Assert(hostSpace.GetPrimary()->m_uuid, hostParticle.m_uuid, "Host space's primary");
	testHandler.Assert(hostSpace.GetPrimaryPosition(), Vector3::Zero(), "Host space primary position");
	testHandler.Assert(hostSpace.GetPrimaryVelocity(), Vector3::Zero(), "Host space primary velocity");

	ScalingSphereBase & scaledSpace2 = *orbitalSystem.CreateScaledSpace(hostParticle, HOST_SPACE_RADIUS / 10.f);

	testHandler.Assert(scaledSpace2.GetRadius(), 0.1f, "Scaled space 2 radius");
	testHandler.Assert(scaledSpace2.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Scaled space 2 host particle");
	testHandler.Assert(scaledSpace2.GetGravityParameter(),
		ScalingSphereBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS / 10.f, HOST_MASS), "Scaled space 2 gravity parameter");
	testHandler.Assert(scaledSpace2.IsInfluencing(), true, "Scaled space 2 is influencing");
	testHandler.Assert(scaledSpace2.GetPrimary()->m_uuid, hostParticle.m_uuid, "Scaled space 2 primary");
	testHandler.Assert(scaledSpace2.GetPrimaryPosition(), Vector3::Zero(), "Scaled space 2 primary position");
	testHandler.Assert(scaledSpace2.GetPrimaryVelocity(), Vector3::Zero(), "Scaled space 2 primary velocity");

	ScalingSphereBase & scaledSpace3 = *orbitalSystem.CreateScaledSpace(hostParticle, HOST_SPACE_RADIUS / 100.f);

	testHandler.Assert(scaledSpace3.GetRadius(), 0.1f, "Scaled space 3 radius");
	testHandler.Assert(scaledSpace3.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Scaled space 3 host particle");
	testHandler.Assert(scaledSpace3.GetGravityParameter(),
		ScalingSphereBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS / 100.f, HOST_MASS), "Scaled space 3 gravity parameter");
	testHandler.Assert(scaledSpace3.IsInfluencing(), true, "Scaled space 3 is influencing");
	testHandler.Assert(scaledSpace3.GetPrimary()->m_uuid, hostParticle.m_uuid, "Scaled space 3 primary");
	testHandler.Assert(scaledSpace3.GetPrimaryPosition(), Vector3::Zero(), "Scaled space 3 primary position");
	testHandler.Assert(scaledSpace3.GetPrimaryVelocity(), Vector3::Zero(), "Scaled space 3 primary velocity");

	bool isException;

	try
	{
		orbitalSystem.CreateScaledSpace(hostParticle, HOST_SPACE_RADIUS / 2.f);
		isException = false;
	}
	catch (ApiException const&)
	{
		isException = true;
	}
	testHandler.Assert(isException, true, "Invalid radius causes exception");

	/*try
	{
		orbitalSystem.CreateParticle(hostSpace, 1e7f, Vector3(0.05f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), false);
		isException = false;
	}
	catch (ApiException const&)
	{
		isException = true;
	}*/

	try
	{
		scaledSpace2.SetRadius(0.5f);
		isException = false;
	}
	catch (ApiException const&)
	{
		isException = true;
	}
	testHandler.Assert(isException, true, "Resizing populated space causes exception");

	try
	{
		ScalingSphereBase & smallestSpace = *hostParticle.GetScalingSphereList().Back();

		float const newTrueRadius = 0.5f * smallestSpace.GetTrueRadius();
		float const newRadius = 0.5f * smallestSpace.GetRadius();

		smallestSpace.SetRadius(newRadius);

		testHandler.Assert(smallestSpace.GetRadius(), newRadius, "New radius");
		testHandler.Assert(smallestSpace.GetTrueRadius(), newTrueRadius, "New true radius");
		testHandler.Assert(scaledSpace3.GetGravityParameter(),
			ScalingSphereBase::ComputeScaledGravityParameter(newTrueRadius, HOST_MASS), "New gravity parameter");

		isException = false;
	}
	catch (ApiException const&)
	{
		isException = true;
	}
	testHandler.Assert(isException, false, "Resizing unpopulated space causes exception");

	const float particleMass = 1e10f;
	const float orbitRadius = 0.5f;
	const float orbitSpeed = hostSpace.CircularOrbitSpeed(orbitRadius);
	const Vector3 particlePosition(orbitRadius, 0.f, 0.f);
	const Vector3 particleVelocity(0.f, orbitSpeed, 0.f);

	ParticleBase & particle = *orbitalSystem.CreateParticle(hostSpace, particleMass, particlePosition, particleVelocity, false);

	testHandler.Assert(particle.GetMass(), particleMass, "PassiveParticle mass");
	testHandler.Assert(particle.GetHostSphere()->m_uuid, hostSpace.m_uuid, "Host space");
	testHandler.Assert(particle.GetPosition(), particlePosition, "PassiveParticle position");
	testHandler.Assert(particle.GetVelocity(), particleVelocity, "PassiveParticle velocity");
	testHandler.Assert(particle.IsInfluencing(), false, "Is particle influencing");
	testHandler.Assert(particle.GetScalingSphereList().Size(), 0ull, "PassiveParticle attached spaces");

	const float particleScaledSpaceRadius = 0.05f;
	const float particleScaledSpaceTrueRadius = HOST_SPACE_RADIUS * particleScaledSpaceRadius;

	ScalingSphereBase & particleScaledSpace = *orbitalSystem.CreateScaledSpace(particle, particleScaledSpaceTrueRadius);

	testHandler.Assert(particleScaledSpace.GetHostParticle()->m_uuid, particle.m_uuid, "PassiveParticle scaled space host particle");
	testHandler.Assert(particleScaledSpace.GetParticleList().size(), 0ull, "PassiveParticle scaled space particle list size");
	testHandler.Assert(particleScaledSpace.GetOuterSpace()->m_uuid, hostSpace.m_uuid, "PassiveParticle scaled space outer space");
	testHandler.Assert(reinterpret_cast<uintptr_t>(particleScaledSpace.GetInnerSpace()), reinterpret_cast<uintptr_t>(nullptr), "PassiveParticle scaled space inner space");

	testHandler.Assert(particleScaledSpace.GetTrueRadius(), particleScaledSpaceTrueRadius, "PassiveParticle scaled space true radius");
	testHandler.Assert(particleScaledSpace.GetRadius(), particleScaledSpaceRadius, "PassiveParticle scaled space radius");

	const float expectedGravityParameter = ScalingSphereBase::ComputeScaledGravityParameter(particleScaledSpaceTrueRadius, HOST_MASS);
	testHandler.Assert(particleScaledSpace.GetGravityParameter(), expectedGravityParameter, "PassiveParticle scaled space gravity parameter");

	testHandler.Assert(particleScaledSpace.IsInfluencing(), false, "PassiveParticle scaled space is influencing");
	testHandler.Assert(particleScaledSpace.GetPrimary()->m_uuid, hostParticle.m_uuid, "PassiveParticle scaled space primary");
	testHandler.Assert(sqrtf(particleScaledSpace.GetPrimaryPosition().SqareMagnitude()), orbitRadius / particleScaledSpaceRadius, "PassiveParticle scaled space primary distance");
	testHandler.Assert(particleScaledSpace.GetPrimaryVelocity(), particleVelocity * -1.f / particleScaledSpaceRadius, "PassiveParticle scaled space primary velocity");

	const float particleScaledSpaceNewRadius = 0.04f;
	const float particleScaledSpaceNewTrueRadius = HOST_SPACE_RADIUS * particleScaledSpaceNewRadius;

	particleScaledSpace.SetRadius(particleScaledSpaceNewRadius);

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
	using InfluencingSphere = OrbitalSystem::InfluencingSpace;

	static constexpr float TRUE_RADIUS_1 = 100.f;
	static constexpr float TRUE_RADIUS_2 = 80.f;
	static constexpr float TRUE_RADIUS_3 = 90.f;

	ScalingSphereList list;
	ScalingSphereList::Iterator insertedIterator;

	testHandler.Assert(list.Empty(), true, "Newly constructed list is empty");

	UniquePtr<ParticleBase> particlePtr = MakeUnique<OrbitalSystem::HostParticle>(1.f);

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

		testHandler.Assert(list.Back()->m_uuid.Get(), uuid2, "Inserted ScalingSphere UUID equals the constructed ScalingSphere's UUID");
		testHandler.Assert(list.Back()->GetTrueRadius(), TRUE_RADIUS_2, "Inserted element TrueRadius");
		testHandler.Assert(list.Back()->GetRadius(), TRUE_RADIUS_2 / TRUE_RADIUS_1, "Inserted element Radius");
	}

	scalingSpherePtr = MakeUnique<InfluencingSphere>(particlePtr.get(), TRUE_RADIUS_3);
	const uint64_t uuid3 = scalingSpherePtr->m_uuid.Get();

	{
		insertedIterator = list.Insert(std::move(scalingSpherePtr));

		(*insertedIterator)->Initialize();
		list.Back()->Initialize();

		testHandler.Assert(list.Size(), 3ull, "Size of list after third insertion");
		testHandler.Assert(reinterpret_cast<uintptr_t>((++list.Begin())->get()), reinterpret_cast<uintptr_t>(insertedIterator->get()),
			"Begin iterator is the new inserted iterator");

		testHandler.Assert((*insertedIterator)->GetTrueRadius(), TRUE_RADIUS_3, "Inserted element TrueRadius");
		testHandler.Assert((*insertedIterator)->GetRadius(), TRUE_RADIUS_3 / TRUE_RADIUS_1, "Inserted element Radius");
		testHandler.Assert(list.Back()->GetRadius(), TRUE_RADIUS_2 / TRUE_RADIUS_3, "Last element new Radius");
	}

	scalingSpherePtr = list.Remove(insertedIterator);

	list.Back()->Initialize();

	testHandler.Assert(nullptr == scalingSpherePtr, false, "ScalingSpherePtr is not null after removal from list");
	testHandler.Assert(scalingSpherePtr->m_uuid.Get(), uuid3, "Removed ScalingSphere UUID equals the constructed ScalingSphere's UUID");
	testHandler.Assert(scalingSpherePtr->GetTrueRadius(), TRUE_RADIUS_3, "Removed ScalingSphere TrueRadius");

	testHandler.Assert(list.Size(), 2ull, "Size of list after removing middle element");
	testHandler.Assert(list.Front()->GetTrueRadius() > list.Back()->GetTrueRadius(), true, "Ordering of elements preserved after removal");
	testHandler.Assert(list.Back()->GetRadius(), TRUE_RADIUS_2 / TRUE_RADIUS_1, "Last element new Radius");
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
