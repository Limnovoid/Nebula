#ifndef NEUTRON_ORBITAL_SYSTEM_H
#define NEUTRON_ORBITAL_SYSTEM_H

#include "NebulaTypes.h"
#include "Exception.h"
#include "Vector3.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

class Particle;
class ScalingSpace;

// --------------------------------------------------------------------------------------------------------------------------------

class System
{
public:
	System(float hostMass, float hostSpaceTrueRadius);

	SharedPtr<ScalingSpace> GetHostSpace() const;

	/// <summary> Create a scaling space at the top level of this system (attached to the system host). </summary>
	/// <param name="trueRadius"> The true radius of the scaling space. </param>
	/// <returns> A pointer to the created scaling space. </returns>
	SharedPtr<ScalingSpace> CreateScalingSpace(float trueRadius);

	/// <summary> Create a particle in the given scaling space. </summary>
	/// <param name="mass"> The mass of the particle. </param>
	/// <param name="position"> The position of the particle, relative/scaled to the host space. </param>
	/// <param name="velocity"> The velocity of the particle, relative/scaled to the host space. </param>
	/// <param name="pHostSpace"> Pointer to the host scaling space. </param>
	/// <returns> A pointer to the created particle. </returns>
	SharedPtr<Particle> CreateParticle(float mass, Vector3 position, Vector3 velocity, SharedPtr<ScalingSpace> pHostSpace);

	/// <summary> Create a scaling space attached to the given particle. </summary>
	/// <param name="trueRadius"> The true radius (in meters) of the scaling space. </param>
	/// <param name="pHostParticle"> Pointer to the host particle. </param>
	/// <returns> A pointer to the created scaling space. </returns>
	/// <exception cref="AssertionException"> Radius is too large. </exception>
	/// <exception cref="AssertionException"> Radius is too small. </exception>
	SharedPtr<ScalingSpace> CreateScalingSpace(float trueRadius, SharedPtr<Particle> pHostParticle);

private:
	float								m_hostMass;			// The magnitude of the point-mass at the centre of the simulation, around which all particles orbit.
	SharedPtr<ScalingSpace>				m_pHostSpace;		// The host scaling space, against whose true radius all other scaling spaces' radii are parameterised.

	std::list<SharedPtr<ScalingSpace>>	m_scalingSpaces;	// Linked list of scaling spaces ordered by radius, highest to lowest.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline SharedPtr<ScalingSpace> System::GetHostSpace() const
{
	return m_pHostSpace;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class SystemTestScript : public ITestScript
{
public:
	SystemTestScript();
	virtual ~SystemTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_ORBITAL_SYSTEM_H
