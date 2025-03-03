#include "Particle.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

Particle::Particle(State const& state, SharedPtr<ScalingSpace> pHostSpace) :
	m_state(state),
	m_pHostSpace(pHostSpace)
{
	assert(false); // TODO - add influencing space
}

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
