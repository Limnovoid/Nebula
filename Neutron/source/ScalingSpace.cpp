#include "ScalingSpace.h"

#include "Particle.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

ScalingSpace::ScalingSpace(float hostMass, float trueRadius, float radius) :
	m_trueRadius(trueRadius),
	m_radius(radius),
	m_gravityParameter(ComputeScaledGravityParameter(trueRadius, hostMass)),
	m_isInfluencing(true)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSpace::ScalingSpace(float trueRadius, bool isInfluencing, SharedPtr<Particle> pHost) :
	m_trueRadius(trueRadius),
	m_gravityParameter(ComputeScaledGravityParameter(trueRadius, pHost->GetState().m_mass)),
	m_isInfluencing(isInfluencing),
	m_pHost(pHost)
{
	assert(false); // TODO - compute radius
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSpace::ComputePrimaryKinetics()
{
	m_primaryPosition = 0.f;
	m_primaryVelocity = 0.f;

	ScalingSpace const* pScalingSpace = this;
	float scaling = 1.f;

	while ((nullptr != pScalingSpace) && !pScalingSpace->m_isInfluencing)
	{
		scaling /= pScalingSpace->m_radius;

		SharedPtr<Particle> pHost = pScalingSpace->m_pHost;

		m_primaryPosition -= pHost->GetState().m_localPosition * scaling;
		m_primaryVelocity -= pHost->GetState().m_localVelocity * scaling;

		pScalingSpace = pHost->GetHostSpace().get();
	}
}

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
