#ifndef NEUTRON_ORBITAL_SYSTEM_H
#define NEUTRON_ORBITAL_SYSTEM_H

#include "NebulaTypes.h"
#include "Exception.h"
#include "Vector3.h"
#include "ScalingSpace.h"
#include "Particle.h"
#include "ITestScript.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

class OrbitalSystem
{
public:
	OrbitalSystem(float hostMass, float hostSpaceTrueRadius);

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
	Particle		m_hostParticle;			// The host particle, around which all other particles in the system orbit.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSpace & OrbitalSystem::GetHostSpace()
{
	return *m_hostParticle.GetHostSpace();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Particle::ScalingSpaceList const& OrbitalSystem::GetScalingSpaces() const
{
	return m_hostParticle.GetScalingSpaceList();
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
