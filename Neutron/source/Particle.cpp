#include "Particle.h"

#include "OrbitalSystem.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

Particle::Particle(State const& state, ScalingSpace * pHostSpace) :
	m_state(state),
	m_pElements(std::move(MakeUnique<Orbit::Elements>())),
	m_pHostSpace(pHostSpace)
{
	Vector3 const positionFromPrimary = m_state.m_localPosition - pHostSpace->GetPrimaryPosition();
	Vector3 const velocityFromPrimary = m_state.m_localVelocity - pHostSpace->GetPrimaryVelocity();

	m_pElements->Compute(pHostSpace->GetGravityParameter(), positionFromPrimary, velocityFromPrimary);

	// TODO - move to OrbitalSystem ...
	/*float const radiusOfInfluence =
		ComputeRadiusOfInfluence(m_pElements->m_semiMajor, m_state.m_mass, pHostSpace->GetPrimary().m_state.m_mass);

	if (kMinimumRadiusOfInfluence < radiusOfInfluence)
	{
		float const trueRadiusOfInfluence = radiusOfInfluence * pHostSpace->GetTrueRadius();

		ScalingSpace & influencingSpace =
			**m_attachedSpaces.Emplace(std::move(MakeUnique<ScalingSpace>(this, trueRadiusOfInfluence)));

		influencingSpace.Initialize(radiusOfInfluence, true);
	}*/
}

// --------------------------------------------------------------------------------------------------------------------------------

Particle::Particle(float mass, float hostSpaceTrueRadius) :
	m_state{ .m_mass = mass },
	m_pHostSpace(nullptr)
{
	m_attachedSpaces.Emplace(std::move(MakeUnique<ScalingSpace>(this, hostSpaceTrueRadius)));
}

// --------------------------------------------------------------------------------------------------------------------------------

void Particle::Set(Vector3 const& position, Vector3 const& velocity, ScalingSpace * pHostSpace)
{
	m_state.m_localPosition = position;
	m_state.m_localVelocity = velocity;
	m_pHostSpace = pHostSpace;

	Vector3 const positionFromPrimary = m_state.m_localPosition - m_pHostSpace->GetPrimaryPosition();
	Vector3 const velocityFromPrimary = m_state.m_localVelocity - m_pHostSpace->GetPrimaryVelocity();

	m_pElements->Compute(m_pHostSpace->GetGravityParameter(), positionFromPrimary, velocityFromPrimary);

	// TODO - move to OrbitalSystem ...
	/*float const radiusOfInfluence =
		ComputeRadiusOfInfluence(m_pElements->m_semiMajor, m_state.m_mass, m_pHostSpace->GetPrimary().m_state.m_mass);

	float const trueRadiusOfInfluence = radiusOfInfluence * pHostSpace->GetTrueRadius();

	ScalingSpace * pInfluence = GetInfluence();
	if (nullptr == pInfluence)
	{
		if (kMinimumRadiusOfInfluence < radiusOfInfluence)
			AddScalingSpace(radiusOfInfluence, trueRadiusOfInfluence);
	}
	else
	{
		if (kMinimumRadiusOfInfluence < radiusOfInfluence)
		{
			pInfluence->SetRadius(trueRadiusOfInfluence);
		}
		else
		{
			assert(false); // TODO - set influence and all smaller spaces to non-influencing ...
		}
	}*/
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
