#include "ScalingSpace.h"

#include "Particle.h"
#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ScalingSpace::ScalingSpace(Particle * pHost, float radius, float trueRadius, bool isInfluencing) :
	m_trueRadius(trueRadius),
	m_radius(radius),
	m_isInfluencing(isInfluencing),
	m_pPrimary(pHost),
	m_pHost(pHost),
	m_pOuterSpace(nullptr),
	m_pInnerSpace(nullptr)
{
	if (!isInfluencing)
		m_pHost = FindPrimary(this);

	m_gravityParameter = ComputeScaledGravityParameter(m_trueRadius, pHost->GetState().m_mass);

	ComputePrimaryKinetics(this, m_primaryPosition, m_primaryVelocity);
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSpace::ComputePrimaryKinetics(ScalingSpace const* pScalingSpace, Vector3 & position, Vector3 & velocity)
{
	position = 0.f;
	velocity = 0.f;

	float scaling = 1.f;

	while ((nullptr != pScalingSpace) && !pScalingSpace->m_isInfluencing)
	{
		assert(0 < pScalingSpace->m_radius);

		scaling /= pScalingSpace->m_radius;

		Particle * pHost = pScalingSpace->m_pHost;

		position -= pHost->GetState().m_localPosition * scaling;
		velocity -= pHost->GetState().m_localVelocity * scaling;

		pScalingSpace = pHost->GetHostSpace();
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

Particle * ScalingSpace::FindPrimary(ScalingSpace const* pScalingSpace)
{
	Particle * pHost;

	do
	{
		pHost = pScalingSpace->m_pHost;

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
