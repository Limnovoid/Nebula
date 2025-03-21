#ifndef NEUTRON_PARTICLE_H
#define NEUTRON_PARTICLE_H

#include "Vector3.h"
#include "Uuid.h"
#include "ScalingSpace.h"
#include "Orbit.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

/* Forward Declarations */
class ScalingSpace;

// --------------------------------------------------------------------------------------------------------------------------------

class Particle
{
	friend class OrbitalSystem;
	friend class ParticleTestScript;

	struct ScalingSpaceListPredicate
	{
		bool operator()(UniquePtr<ScalingSpace> const& lhs, UniquePtr<ScalingSpace> const& rhs);
	};

public:
	using ScalingSpaceList = SortedList<UniquePtr<ScalingSpace>, ScalingSpaceListPredicate>;

	struct State
	{
		float		m_mass;					// Mass of the particle.
		Vector3		m_localPosition;		// Position relative/scaled to the host space.
		Vector3		m_localVelocity;		// Velocity relative/scaled to the host space.
	};

	/// <summary> Compute the radius of influence. Result has the same units as the orbit radius provided. </summary>
	/// <param name="orbitRadius"> The orbit radius. </param>
	/// <param name="particleMass"> The mass of the orbiting particle whose radius of influence is being computed. </param>
	/// <param name="primaryMass"> The mass of the orbit primary. </param>
	/// <returns> The radius of influence. </returns>
	static float ComputeRadiusOfInfluence(float orbitRadius, float particleMass, float primaryMass);

	Particle(State const& state, ScalingSpace * pHostSpace);

	State const& GetState() const;
	ScalingSpace * GetHostSpace();
	ScalingSpaceList const& GetScalingSpaceList() const;

	Uuid								m_uuid;

private:
	Particle(float mass, float hostSpaceTrueRadius);

	State								m_state;						// Physical state of the particle.

	UniquePtr<Orbit::Elements>			m_pElements;					// Pointer to the orbit elements.

	ScalingSpace *						m_pHostSpace;					// Pointer to the scaling space in which this particle is moving, or the orbital system's host space if this particle is the system host particle.
	ScalingSpaceList					m_attachedSpaces;				// List of pointers to scaling spaces attached to this particle.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline Particle::State const& Particle::GetState() const
{
	return m_state;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSpace * Particle::GetHostSpace()
{
	return m_pHostSpace;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Particle::ScalingSpaceList const& Particle::GetScalingSpaceList() const
{
	return m_attachedSpaces;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ParticleTestScript : public ITestScript
{
public:
	ParticleTestScript();
	virtual ~ParticleTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_PARTICLE_H
