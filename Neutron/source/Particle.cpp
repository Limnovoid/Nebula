#include "Particle.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

Particle::Particle(State state, SharedPtr<ScalingSpace> pHostSpace) :
	m_state(std::move(state)),
	m_pHostSpace(pHostSpace)
{
	assert(false); // TODO - add influencing space
}

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
