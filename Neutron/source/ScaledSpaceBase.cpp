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
	m_gravityParameter = ComputeScaledGravityParameter(m_trueRadius, GetPrimary()->GetMass());
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScaledSpaceBase::SetRadius(float radius)
{
	API_ASSERT_THROW(nullptr != m_pOuterSpace, RESULT_CODE_INVALID_STATE, "Cannot set the relative radius of the host space");

	API_ASSERT_THROW(m_particles.empty() && (nullptr == m_pInnerSpace), RESULT_CODE_INVALID_STATE,
		"Cannot resize a Scaled Space containing any smaller spaces or particles");

	API_ASSERT_THROW((kMinimumScalingSpaceRadius <= radius) && (radius < kMaximumScalingSpaceRadius), RESULT_CODE_INVALID_PARAMETER,
		Fmt::Format("Radius must be a value in the range [{}, {})", kMinimumScalingSpaceRadius, kMaximumScalingSpaceRadius));

	m_trueRadius = m_pOuterSpace->m_trueRadius * radius;

	Initialize(radius);
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
