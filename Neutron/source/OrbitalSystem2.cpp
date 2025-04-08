#include "OrbitalSystem2.h"

#include "TestHandler.h"
#include "Exception.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

OrbitalSystem2::OrbitalSystem2(float hostMass, float hostSpaceTrueRadius) :
	m_pHostParticle(std::move(MakeUnique<HostParticle>(hostMass)))
{
	InfluencingSpace * pHostSpace = m_pHostParticle->EmplaceScaledSpace<InfluencingSpace>(hostSpaceTrueRadius);
	pHostSpace->Initialize(1.f);
}

// --------------------------------------------------------------------------------------------------------------------------------

ScaledSpaceBase & OrbitalSystem2::CreateScaledSpace(ParticleBase & hostParticle, float trueRadius)
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

	return *CreateScaledSpace(&hostParticle, trueRadius, isInfluencing);
}

// --------------------------------------------------------------------------------------------------------------------------------

ScaledSpaceBase * OrbitalSystem2::CreateScaledSpace(ParticleBase * pHostParticle, float trueRadius, bool isInfluencing)
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

	pNewScaledSpace->Initialize(radius);

	return pNewScaledSpace;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem2::HostParticle::HostParticle(float hostMass) :
	ParticleBase(nullptr, hostMass)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem2::Particle::Particle(ScaledSpaceBase * pHostSpace, float hostMass) :
	ParticleBase(pHostSpace, hostMass)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitalSystem2::InfluencingParticle::InfluencingParticle(ScaledSpaceBase * pHostSpace, float hostMass) :
	Particle(pHostSpace, hostMass)
{
	m_elements.Compute(pHostSpace->GetGravityParameter(), m_position - pHostSpace->GetPrimaryPosition(),
		m_velocity - pHostSpace->GetPrimaryVelocity());

	float const radiusOfInfluence = ComputeRadiusOfInfluence(m_elements.m_semiMajor, m_mass, pHostSpace->GetPrimary().m_mass);

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

	ParticleBase & hostParticle = orbitalSystem.GetHostParticle();

	testHandler.Assert(reinterpret_cast<uintptr_t>(hostParticle.GetHostSpace()), reinterpret_cast<uintptr_t>(nullptr),
		"Host particle's host space is nullptr");

	testHandler.Assert(hostParticle.GetMass(), HOST_MASS, "Host particle mass");
	testHandler.Assert(hostParticle.GetPosition(), Vector3::Zero(), "Host particle position");
	testHandler.Assert(hostParticle.GetVelocity(), Vector3::Zero(), "Host particle velocity");
	testHandler.Assert(hostParticle.IsInfluencing(), true, "Host particle is influencing");

	ScaledSpaceBase & hostSpace = orbitalSystem.GetHostSpace();

	testHandler.Assert(hostSpace.m_uuid, hostParticle.GetAttachedSpaces().Front().m_uuid,
		"Host space is host particle's first attached space");

	testHandler.Assert(hostSpace.GetRadius(), 1.f, "Host space radius");
	testHandler.Assert(hostSpace.GetTrueRadius(), HOST_SPACE_RADIUS, "Host space true radius");
	testHandler.Assert(hostSpace.GetHostParticle()->m_uuid, hostParticle.m_uuid, "Host space's host particle");
	testHandler.Assert(hostSpace.GetGravityParameter(),
		ScaledSpaceBase::ComputeScaledGravityParameter(HOST_SPACE_RADIUS, HOST_MASS), "Host space gravity parameter");
	testHandler.Assert(hostSpace.IsInfluencing(), true, "Host space is influencing");
	testHandler.Assert(hostSpace.GetPrimary().m_uuid, hostParticle.m_uuid, "Host space's primary");
	testHandler.Assert(hostSpace.GetPrimaryPosition(), Vector3::Zero(), "Host space primary position");
	testHandler.Assert(hostSpace.GetPrimaryVelocity(), Vector3::Zero(), "Host space primary velocity");

}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
