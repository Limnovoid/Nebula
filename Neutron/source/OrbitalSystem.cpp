#include "OrbitalSystem.h"

#include "ScalingSpace.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

System::System(float hostMass, float hostSpaceTrueRadius) :
	m_hostMass(hostMass),
	m_pHostSpace(MakeShared<ScalingSpace>(hostSpaceTrueRadius, hostMass))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

SharedPtr<Particle> System::CreateParticle(Particle::State state, SharedPtr<ScalingSpace> pHostSpace)
{
	SharedPtr<Particle> pNewParticle = MakeShared<Particle>(std::move(state), pHostSpace);

	return pNewParticle;
}

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
