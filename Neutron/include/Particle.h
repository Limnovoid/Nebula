#ifndef NEUTRON_PARTICLE_H
#define NEUTRON_PARTICLE_H

#include "Vector3.h"
#include "Uuid.h"
#include "ScalingSpace.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

/* Forward Declarations */
class ScalingSpace;

// --------------------------------------------------------------------------------------------------------------------------------

class Particle
{
	friend class System;

public:
	using List = std::list<SharedPtr<Particle>>;

	struct State
	{
		float		m_mass;					// Mass of the particle.
		Vector3		m_localPosition;		// Position relative/scaled to the host space.
		Vector3		m_localVelocity;		// Velocity relative/scaled to the host space.
	};

	Particle(State const& state, SharedPtr<ScalingSpace> pHostSpace);

	State const& GetState() const;

	SharedPtr<const ScalingSpace> GetHostSpace() const;

	Uuid								m_uuid;

private:
	State								m_state;						// Physical state of the particle.

	SharedPtr<ScalingSpace>				m_pHostSpace;					// Pointer to the scaling space in which this particle is moving.
	ScalingSpace::List					m_attachedSpaces;				// List of pointers to scaling spaces attached to this particle.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline Particle::State const& Particle::GetState() const
{
	return m_state;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline SharedPtr<const ScalingSpace> Particle::GetHostSpace() const
{
	return m_pHostSpace;
}

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_PARTICLE_H
