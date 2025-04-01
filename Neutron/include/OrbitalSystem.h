#ifndef NEUTRON_ORBITAL_SYSTEM_H
#define NEUTRON_ORBITAL_SYSTEM_H

#include "NebulaTypes.h"
#include "Exception.h"
#include "Vector3.h"
#include "ScalingSpace.h"
#include "Particle.h"
#include "ITestScript.h"
#include "NeutronTime.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

class OrbitalSystem
{
public:
	OrbitalSystem(float hostMass, float hostSpaceTrueRadius);

	/// <summary> Compute the radius of influence. Result has the same units as the orbit radius provided. </summary>
	/// <param name="orbitRadius"> The orbit radius. </param>
	/// <param name="particleMass"> The mass of the orbiting particle whose radius of influence is being computed. </param>
	/// <param name="primaryMass"> The mass of the orbiter's primary. </param>
	/// <returns> The radius of influence. </returns>
	static float ComputeRadiusOfInfluence(float orbitRadius, float particleMass, float primaryMass);

	void Reset(float hostMass, float hostSpaceTrueRadius);

	void OnUpdate(Time::Microseconds dT);

	ScalingSpace & GetHostSpace();
	Particle::ScalingSpaceList const& GetScalingSpaces() const;

	/// <summary> Create a scaling space at the top level of this system (attached to the system host). </summary>
	/// <param name="trueRadius"> The true radius of the scaling space. </param>
	/// <returns> A pointer to the created scaling space. </returns>
	ScalingSpace & CreateScalingSpace(float trueRadius);

	/// <summary> Create a particle in the given scaling space. </summary>
	/// <param name="mass"> The mass of the particle. </param>
	/// <param name="position"> The position of the particle, relative/scaled to the host space. </param>
	/// <param name="velocity"> The velocity of the particle, relative/scaled to the host space. </param>
	/// <param name="pHostSpace"> Pointer to the host scaling space. </param>
	/// <returns> A pointer to the created particle. </returns>
	Particle & CreateParticle(float mass, Vector3 position, Vector3 velocity, ScalingSpace & hostSpace);

	/// <summary> Create a scaling space attached to the given particle. </summary>
	/// <param name="radius"> The true radius of the scaling space (in meters). </param>
	/// <param name="pHostParticle"> Pointer to the host particle. </param>
	/// <returns> A pointer to the created scaling space. </returns>
	/// <exception cref="AssertionException"> Radius is too large. </exception>
	/// <exception cref="AssertionException"> Radius is too small. </exception>
	ScalingSpace & CreateScalingSpace(float trueRadius, Particle & hostParticle);

private:
	struct ParticleUpdateQueuePredicate
	{
		bool operator()(Particle *const& lhs, Particle *const& rhs);
	};

	using ParticleUpdateQueue = SortedList<Particle *, ParticleUpdateQueuePredicate>;

	/// <summary>
	/// Create a scaling space in the host particle's scaling space list and initialize it's outer/inner scaling space pointers.
	/// NOTE: does not call Initialize() on the created scaling space.
	/// </summary>
	/// <returns> Reference to the created scaling space. </returns>
	static Particle::ScalingSpaceList::iterator EmplaceScalingSpace(float trueRadius, Particle & hostParticle);

	/// <summary>
	/// Initialize an existing scaling space from its true radius and position in its host's scaling space list.
	/// Computes new radius and whether it is influencing from the true radius and the host's existing space of influence.
	/// </summary>
	/// <param name="scalingSpace"> The scaling space to initialize. </param>
	static void InitializeScalingSpace(ScalingSpace & scalingSpace);

	/// <summary>
	/// Recompute the space's radius relative to the outer space, assuming constant true radius.
	/// If not influencing, recomputes all particle orbits (as primary kinetics are affected by rescaling).
	/// </summary>
	/// <param name="scalingSpace"> The scaling space to rescale. </param>
	static void RecomputeRadius(ScalingSpace & scalingSpace);

	/// <summary> Initialize an existing particle in the given scaling space. </summary>
	/// <param name="particle"> The particle to initialize. </param>
	/// <param name="position"> The particle's new position, relative to the new scaling space. </param>
	/// <param name="velocity"> The particle's new velocity, relative to the new scaling space. </param>
	/// <param name="scalingSpace"> The particle's new scaling space. </param>
	static void InitializeParticle(Particle & particle, Vector3 const& position, Vector3 const& velocity, ScalingSpace & scalingSpace);

	/// <returns> Whether a particle at the given radial distance from the given scaling space should enter the scaling space. </returns>

	/// <summary> Whether a particle at the given radial distance from the given scaling space should enter the scaling space. </summary>
	/// <param name="particleRadialDistance"> The particle's distance from the scaling space host, scaled to the scaling space's outer space. </param>
	/// <param name="scalingSpace"> The scaling space. </param>
	/// <returns></returns>
	static bool ShouldDescend(float particleRadialDistance, ScalingSpace const& scalingSpace);
	static bool ShouldAscend(float particleRadialDistance);

	UniquePtr<Particle>		m_pHostParticle;			// The host particle, around which all other particles in the system orbit.

	ParticleUpdateQueue		m_particleUpdateQueue;		// The particle update queue.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline float OrbitalSystem::ComputeRadiusOfInfluence(float orbitRadius, float particleMass, float primaryMass)
{
	return orbitRadius * powf(particleMass / primaryMass, 2.f / 5.f);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSpace & OrbitalSystem::GetHostSpace()
{
	return *m_pHostParticle->GetHostSpace();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Particle::ScalingSpaceList const& OrbitalSystem::GetScalingSpaces() const
{
	return m_pHostParticle->GetScalingSpaceList();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem::ShouldDescend(float particleRadialDistance, ScalingSpace const& scalingSpace)
{
	return particleRadialDistance <= scalingSpace.m_radius;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem::ShouldAscend(float particleRadialDistance)
{
	return kScalingSpaceEscapeRadius < particleRadialDistance;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline bool OrbitalSystem::ParticleUpdateQueuePredicate::operator()(Particle *const& lhs, Particle *const& rhs)
{
	assert(false); // TODO !
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class OrbitalSystemTestScript : public ITestScript
{
public:
	OrbitalSystemTestScript();
	virtual ~OrbitalSystemTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_ORBITAL_SYSTEM_H
