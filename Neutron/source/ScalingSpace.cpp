#include "ScalingSpace.h"

#include "Particle.h"
#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ScalingSpace::ScalingSpace(Particle * pHost, float trueRadius) :
	m_trueRadius(trueRadius),
	m_radius(1.f),
	m_isInfluencing(true),
	m_gravityParameter(0.f),
	m_pPrimary(pHost),
	m_pHost(pHost),
	m_pOuterSpace(nullptr),
	m_pInnerSpace(nullptr)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSpace::Initialize(float radius, bool isInfluencing)
{
	m_radius = radius;
	m_isInfluencing = isInfluencing;

	if (!isInfluencing)
		m_pPrimary = FindPrimary(this);

	m_gravityParameter = ComputeScaledGravityParameter(m_trueRadius, m_pPrimary->GetState().m_mass);

	ComputePrimaryKinetics(this, m_primaryPosition, m_primaryVelocity);
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSpace::ComputePrimaryKinetics(ScalingSpace const* pScalingSpace, Vector3 & position, Vector3 & velocity)
{
	assert(nullptr != pScalingSpace);
	assert(0 < pScalingSpace->m_trueRadius);

	position = 0.f;
	velocity = 0.f;

	float scaling = 1.f;

	while ((nullptr != pScalingSpace) && !pScalingSpace->m_isInfluencing)
	{
		Particle * pHost = pScalingSpace->m_pHost;
		assert(nullptr != pHost);

		ScalingSpace const* pHostSpace = pHost->GetHostSpace();
		assert(nullptr != pHostSpace); // nullptr would mean the host particle is the system host, but all scaling spaces on the system host should be influencing so this should never happen.
		assert(0 < pHostSpace->m_trueRadius);

		scaling *= (pHostSpace->m_trueRadius / pScalingSpace->m_trueRadius);

		position -= pHost->GetState().m_localPosition * scaling;
		velocity -= pHost->GetState().m_localVelocity * scaling;

		pScalingSpace = pHostSpace;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

Particle * ScalingSpace::FindPrimary(ScalingSpace const* pScalingSpace)
{
	Particle * pHost;
	do
	{
		pHost = pScalingSpace->m_pHost;

		assert(nullptr != pHost);

		pScalingSpace = pHost->GetHostSpace();
	}
	while ((nullptr != pScalingSpace) && !pScalingSpace->m_isInfluencing);

	return pHost;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpaceTestScript::ScalingSpaceTestScript() :
	ITestScript("ScalingSpace")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpaceTestScript::~ScalingSpaceTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSpaceTestScript::RunImpl(TestHandler & testHandler)
{
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
