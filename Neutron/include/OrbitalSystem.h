#ifndef NEUTRON_ORBITAL_SYSTEM_H
#define NEUTRON_ORBITAL_SYSTEM_H

#include "NebulaTypes.h"
#include "Particle.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

class ScalingSpace;

// --------------------------------------------------------------------------------------------------------------------------------

class System
{
public:
	System(float hostMass, float hostSpaceTrueRadius);

	SharedPtr<Particle> CreateParticle(Particle::State state, SharedPtr<ScalingSpace> pHostSpace);

private:
	float								m_hostMass;
	SharedPtr<ScalingSpace>				m_pHostSpace;

	std::list<SharedPtr<ScalingSpace>>	m_scalingSpaces; // Linked list of scaling spaces ordered by radius, highest to lowest.
};

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_ORBITAL_SYSTEM_H
