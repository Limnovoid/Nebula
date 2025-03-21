#include "Particle.h"

#include "OrbitalSystem.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

Particle::Particle(State const& state, ScalingSpace * pHostSpace) :
	m_state(state),
	m_pElements(std::move(MakeUnique<Orbit::Elements>())),
	m_pHostSpace(pHostSpace)
{
	Vector3 const positionFromPrimary = m_state.m_localPosition - m_pHostSpace->GetPrimaryPosition();
	Vector3 const velocityFromPrimary = m_state.m_localVelocity - m_pHostSpace->GetPrimaryVelocity();

	m_pElements->Compute(m_pHostSpace->GetGravityParameter(), positionFromPrimary, velocityFromPrimary);

	float const radiusOfInfluence =
		ComputeRadiusOfInfluence(m_pElements->m_semiMajor, m_state.m_mass, m_pHostSpace->GetPrimary().m_state.m_mass);

	if (kMinimumRadiusOfInfluence < radiusOfInfluence)
	{
		float const trueRadiusOfInfluence = radiusOfInfluence * pHostSpace->GetTrueRadius();

		m_attachedSpaces.Emplace(std::move(MakeUnique<ScalingSpace>(this, radiusOfInfluence, trueRadiusOfInfluence, true)));
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

Particle::Particle(float mass, float hostSpaceTrueRadius) :
	m_state{ .m_mass = mass }
{
	m_attachedSpaces.Emplace(std::move(MakeUnique<ScalingSpace>(this, 1.f, hostSpaceTrueRadius, true)));
	m_pHostSpace = (*m_attachedSpaces.begin()).get();
}

// --------------------------------------------------------------------------------------------------------------------------------

float Particle::ComputeRadiusOfInfluence(float orbitRadius, float particleMass, float primaryMass)
{
	return orbitRadius * powf(particleMass / primaryMass, 2.f / 5.f);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

bool Particle::ScalingSpaceListPredicate::operator()(UniquePtr<ScalingSpace> const& lhs, UniquePtr<ScalingSpace> const& rhs)
{
	return lhs->GetTrueRadius() > rhs->GetTrueRadius();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

ParticleTestScript::ParticleTestScript() :
	ITestScript("Particle")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleTestScript::~ParticleTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void ParticleTestScript::RunImpl(TestHandler & testHandler)
{
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
