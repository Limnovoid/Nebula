#include "ScaledSpaceBase.h"

#include "ParticleBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ScaledSpaceBase::ScaledSpaceBase(ParticleBase * pHostParticle, float trueRadius) :
	m_pHostParticle(pHostParticle),
	m_pOuterSpace(nullptr),
	m_pInnerSpace(nullptr),
	m_trueRadius(trueRadius),
	m_radius(1.f),
	m_gravityParameter(0.f)
{
	assert(nullptr != m_pHostParticle);
	assert(0.f < m_trueRadius);
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScaledSpaceBase::Initialize(float radius)
{
	m_radius = radius;
	m_gravityParameter = ComputeScaledGravityParameter(m_trueRadius, GetPrimary().GetMass());
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
