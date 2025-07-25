#include "OrbitalSystem2.h"

#include "TestHandler.h"
#include "Exception.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

OrbitalSystem2::OrbitalSystem2(float hostMass, float hostSpaceTrueRadius) :
	m_pHostParticle(std::move(MakeUnique<HostParticle>(*this, hostMass)))
{
	InfluencingSpace * pHostSpace = m_pHostParticle->EmplaceScaledSpace<InfluencingSpace>(hostSpaceTrueRadius);
	pHostSpace->Initialize(1.f);
}

// --------------------------------------------------------------------------------------------------------------------------------

ScaledSpaceBase * OrbitalSystem2::CreateScaledSpace(ParticleBase & hostParticle, float trueRadius)
{
	bool isInfluencing = false;

	if (!hostParticle.m_attachedSpaces.empty())
	{
		if (trueRadius < hostParticle.m_attachedSpaces.Back().m_trueRadius)
		{
			isInfluencing = hostParticle.IsInfluencing();
		}
		else if (trueRadius < hostParticle.m_attachedSpaces.Front().m_trueRadius)
		{
			throw ApiException(RESULT_CODE_INVALID_PARAMETER,
				"True radius must be greater or less than all existing scaled spaces on the particle.");
		}
	}

	return CreateScaledSpaceImpl(&hostParticle, trueRadius, isInfluencing);
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleBase * OrbitalSystem2::CreateParticle(ScaledSpaceBase & hostSpace, float mass, Vector3 position, Vector3 velocity,
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
		pNewParticle = MakeUnique<InfluencingParticle>(*this, &hostSpace, mass, position, velocity);
	else
		pNewParticle = MakeUnique<Particle>(*this, &hostSpace, mass, position, velocity);

	return pNewParticle.get();
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleBase * OrbitalSystem2::CreateParticle(ScaledSpaceBase & hostSpace, float mass, Vector3 position, bool isInfluencing)
{
	hostSpace.CircularOrbitSpeed(sqrtf(position.SqareMagnitude()));

	assert(false); // TODO - circular orbit velocity direction
	Vector3 circularOrbitVelocity;

	return CreateParticle(hostSpace, mass, position, circularOrbitVelocity, isInfluencing);
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem2::DestroyParticle(ParticleBase * pParticleBase)
{
	API_ASSERT_THROW(m_pHostParticle->m_uuid != pParticleBase->m_uuid, RESULT_CODE_INVALID_PARAMETER, "Cannot destroy the host particle");

	ScaledSpaceBase::ParticleList & particleList = pParticleBase->m_pHostSpace->m_particles;

	for (ScaledSpaceBase::ParticleList::const_iterator citerator = particleList.cbegin(); particleList.cend() != citerator; ++citerator)
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

ScaledSpaceBase * OrbitalSystem2::CreateScaledSpaceImpl(ParticleBase * pHostParticle, float trueRadius, bool isInfluencing)
{
	ScaledSpaceBase * pNewScaledSpace = nullptr;

	if (isInfluencing)
	{
		pNewScaledSpace = pHostParticle->EmplaceScaledSpace<InfluencingSpace>(trueRadius);

		assert((nullptr == pNewScaledSpace->m_pOuterSpace) || pNewScaledSpace->m_pOuterSpace->IsInfluencing());
	}
	else
	{
		pNewScaledSpace = pHostParticle->EmplaceScaledSpace<NonInfluencingSpace>(trueRadius);

		assert(nullptr != pNewScaledSpace->m_pOuterSpace); // The highest space should never be non-influencing.
		assert(!pNewScaledSpace->m_pOuterSpace->IsInfluencing() ||
			(pNewScaledSpace->m_pOuterSpace->m_pHostParticle != pNewScaledSpace->m_pHostParticle));
	}

	float const radius = (nullptr == pNewScaledSpace->m_pOuterSpace) ? 1.f :
		trueRadius / pNewScaledSpace->m_pOuterSpace->m_trueRadius;

	if (!((kMinimumScalingSpaceRadius <= radius) && (radius < kMaximumScalingSpaceRadius)))
	{
		assert(false); // TODO - handle invalid size ...

		API_ASSERT_THROW(false, RESULT_CODE_INVALID_PARAMETER,
			Fmt::Format("Radius must be a value in the range [{}, {})", kMinimumScalingSpaceRadius, kMaximumScalingSpaceRadius));
	}

	pNewScaledSpace->Initialize(radius);

	return pNewScaledSpace;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem2::HostParticle::HostParticle(OrbitalSystem2 & orbitalSystem, float hostMass) :
	ParticleBase(orbitalSystem, nullptr, hostMass)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem2::Particle::Particle(OrbitalSystem2 & orbitalSystem, ScaledSpaceBase * pHostSpace, float mass, Vector3 position, Vector3 velocity) :
	ParticleBase(orbitalSystem, pHostSpace, mass),
	m_position(position),
	m_velocity(velocity),
	m_pOrbit(MakeUnique<Orbit>())
{
	Orbit::Elements &elements = m_pOrbit->GetCurrentSection().m_elements;

	elements.Compute(pHostSpace->GetGravityParameter(), position - pHostSpace->GetPrimaryPosition(),
		velocity - pHostSpace->GetPrimaryVelocity());
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem2::InfluencingParticle::InfluencingParticle(OrbitalSystem2 & orbitalSystem, ScaledSpaceBase * pHostSpace, float mass, Vector3 position, Vector3 velocity) :
	Particle(orbitalSystem, pHostSpace, mass, position, velocity)
{
	Orbit::Elements &elements = m_pOrbit->GetCurrentSection().m_elements;

	float const radiusOfInfluence = ComputeRadiusOfInfluence(elements.m_semiMajor, mass, pHostSpace->GetPrimary()->m_mass);

	float const trueRadiusOfInfluence = radiusOfInfluence * pHostSpace->GetTrueRadius();
	m_pSpaceOfInfluence = EmplaceScaledSpace<InfluencingSpace>(trueRadiusOfInfluence);

	m_pSpaceOfInfluence->Initialize(radiusOfInfluence);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem2TestScript::OrbitalSystem2TestScript() :
	ITestScript("OrbitalSystem2")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem2TestScript::~OrbitalSystem2TestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitalSystem2TestScript::RunImpl(TestHandler & testHandler)
{
	static constexpr float HOST_MASS = 1e30f;
	static constexpr float HOST_SPACE_RADIUS = 8e12f;

	OrbitalSystem2 orbitalSystem(HOST_MASS, HOST_SPACE_RADIUS);

	ParticleBase & hostParticle = *orbitalSystem.GetHostParticle();

	testHandler.Assert(reinterpret_cast<uintptr_t>(hostParticle.GetHostSpace()), reinterpret_cast<uintptr_t>(nullptr),
		"Host particle's host space is nullptr");

	testHandler.Assert(hostParticle.GetMass(), HOST_MASS, "Host particle mass");
	testHandler.Assert(hostParticle.GetPosition(), Vector3::Zero(), "Host particle position");
	testHandler.Assert(hostParticle.GetVelocity(), Vector3::Zero(), "Host particle velocity");
	testHandler.Assert(hostParticle.IsInfluencing(), true, "Host particle is influencing");

	ScaledSpaceBase & hostSpace = *orbitalSystem.GetHostSpace();

	testHandler.Assert(hostSpace.m_uuid, hostParticle.GetAttachedSpaces().Front().m_uuid,
		"Host space is host particle's first attached space");

	testHandler.Assert(hostSpace.GetRadius(), 1.f, "Host space radius");
	testHandler.Assert(hostSpace.GetTrueRadius(), HOST_SPACE_RADIUS, "Host space true radius");
	testHandler.Assert(hostSpace.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Host space's host particle");
	testHandler.Assert(hostSpace.GetGravityParameter(),
		ScaledSpaceBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS, HOST_MASS), "Host space gravity parameter");
	testHandler.Assert(hostSpace.IsInfluencing(), true, "Host space is influencing");
	testHandler.Assert(hostSpace.GetPrimary()->m_uuid, hostParticle.m_uuid, "Host space's primary");
	testHandler.Assert(hostSpace.GetPrimaryPosition(), Vector3::Zero(), "Host space primary position");
	testHandler.Assert(hostSpace.GetPrimaryVelocity(), Vector3::Zero(), "Host space primary velocity");

	ScaledSpaceBase & scaledSpace2 = *orbitalSystem.CreateScaledSpace(hostParticle, HOST_SPACE_RADIUS / 10.f);

	testHandler.Assert(scaledSpace2.GetRadius(), 0.1f, "Scaled space 2 radius");
	testHandler.Assert(scaledSpace2.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Scaled space 2 host particle");
	testHandler.Assert(scaledSpace2.GetGravityParameter(),
		ScaledSpaceBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS / 10.f, HOST_MASS), "Scaled space 2 gravity parameter");
	testHandler.Assert(scaledSpace2.IsInfluencing(), true, "Scaled space 2 is influencing");
	testHandler.Assert(scaledSpace2.GetPrimary()->m_uuid, hostParticle.m_uuid, "Scaled space 2 primary");
	testHandler.Assert(scaledSpace2.GetPrimaryPosition(), Vector3::Zero(), "Scaled space 2 primary position");
	testHandler.Assert(scaledSpace2.GetPrimaryVelocity(), Vector3::Zero(), "Scaled space 2 primary velocity");

	ScaledSpaceBase & scaledSpace3 = *orbitalSystem.CreateScaledSpace(hostParticle, HOST_SPACE_RADIUS / 100.f);

	testHandler.Assert(scaledSpace3.GetRadius(), 0.1f, "Scaled space 3 radius");
	testHandler.Assert(scaledSpace3.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Scaled space 3 host particle");
	testHandler.Assert(scaledSpace3.GetGravityParameter(),
		ScaledSpaceBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS / 100.f, HOST_MASS), "Scaled space 3 gravity parameter");
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
		ScaledSpaceBase & smallestSpace = hostParticle.GetAttachedSpaces().Back();

		float const newTrueRadius = 0.5f * smallestSpace.GetTrueRadius();
		float const newRadius = 0.5f * smallestSpace.GetRadius();

		smallestSpace.SetRadius(newRadius);

		testHandler.Assert(smallestSpace.GetRadius(), newRadius, "New radius");
		testHandler.Assert(smallestSpace.GetTrueRadius(), newTrueRadius, "New true radius");
		testHandler.Assert(scaledSpace3.GetGravityParameter(),
			ScaledSpaceBase::ComputeScaledGravityParameter(newTrueRadius, HOST_MASS), "New gravity parameter");

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

	testHandler.Assert(particle.GetMass(), particleMass, "Particle mass");
	testHandler.Assert(particle.GetHostSpace()->m_uuid, hostSpace.m_uuid, "Host space");
	testHandler.Assert(particle.GetPosition(), particlePosition, "Particle position");
	testHandler.Assert(particle.GetVelocity(), particleVelocity, "Particle velocity");
	testHandler.Assert(particle.IsInfluencing(), false, "Is particle influencing");
	testHandler.Assert(particle.GetAttachedSpaces().size(), 0ull, "Particle attached spaces");

	const float particleScaledSpaceRadius = 0.05f;
	const float particleScaledSpaceTrueRadius = HOST_SPACE_RADIUS * particleScaledSpaceRadius;

	ScaledSpaceBase & particleScaledSpace = *orbitalSystem.CreateScaledSpace(particle, particleScaledSpaceTrueRadius);

	testHandler.Assert(particleScaledSpace.GetHostParticle()->m_uuid, particle.m_uuid, "Particle scaled space host particle");
	testHandler.Assert(particleScaledSpace.GetParticleList().size(), 0ull, "Particle scaled space particle list size");
	testHandler.Assert(particleScaledSpace.GetOuterSpace()->m_uuid, hostSpace.m_uuid, "Particle scaled space outer space");
	testHandler.Assert(reinterpret_cast<uintptr_t>(particleScaledSpace.GetInnerSpace()), reinterpret_cast<uintptr_t>(nullptr), "Particle scaled space inner space");

	testHandler.Assert(particleScaledSpace.GetTrueRadius(), particleScaledSpaceTrueRadius, "Particle scaled space true radius");
	testHandler.Assert(particleScaledSpace.GetRadius(), particleScaledSpaceRadius, "Particle scaled space radius");

	const float expectedGravityParameter = ScaledSpaceBase::ComputeScaledGravityParameter(particleScaledSpaceTrueRadius, HOST_MASS);
	testHandler.Assert(particleScaledSpace.GetGravityParameter(), expectedGravityParameter, "Particle scaled space gravity parameter");

	testHandler.Assert(particleScaledSpace.IsInfluencing(), false, "Particle scaled space is influencing");
	testHandler.Assert(particleScaledSpace.GetPrimary()->m_uuid, hostParticle.m_uuid, "Particle scaled space primary");
	testHandler.Assert(sqrtf(particleScaledSpace.GetPrimaryPosition().SqareMagnitude()), orbitRadius / particleScaledSpaceRadius, "Particle scaled space primary distance");
	testHandler.Assert(particleScaledSpace.GetPrimaryVelocity(), particleVelocity * -1.f / particleScaledSpaceRadius, "Particle scaled space primary velocity");

	const float particleScaledSpaceNewRadius = 0.04f;
	const float particleScaledSpaceNewTrueRadius = HOST_SPACE_RADIUS * particleScaledSpaceNewRadius;

	particleScaledSpace.SetRadius(particleScaledSpaceNewRadius);

	testHandler.Assert(particleScaledSpace.GetTrueRadius(), particleScaledSpaceNewTrueRadius, "Particle scaled space new true radius");
	testHandler.Assert(particleScaledSpace.GetRadius(), particleScaledSpaceNewRadius, "Particle scaled space new radius");
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
