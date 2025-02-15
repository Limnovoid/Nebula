#ifndef NEUTRON_SCALING_SPACE_H
#define NEUTRON_SCALING_SPACE_H

#include "Vector3.h"
#include "Constants.h"
#include "Maths.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

namespace Orbital // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

class Particle;

// --------------------------------------------------------------------------------------------------------------------------------

class ScalingSpace
{
public:
	/// <summary>
	/// Compute the locally scaled gravitational parameter of the local primary.
	/// </summary>
	/// <param name="trueRadius"></param>
	/// <param name="hostMass"></param>
	/// <returns></returns>
	static float ComputeScaledGravityParameter(float trueRadius, float hostMass);

	ScalingSpace(float trueRadius, float hostMass); // Warning - use to construct the system host space only (see OrbitalSystem).
	ScalingSpace(float trueRadius, bool isInfluencing, SharedPtr<Particle> pHost);

	bool IsInfluencing() const;

private:
	/// <summary> Compute the position and velocity of the local primary relative/scaled to this scaling space. </summary>
	void ComputePrimaryKinetics();

	float							m_trueRadius;			// Radius in meters.
	float							m_radius;				// Radius relative to superior scaling space.

	float							m_gravityParameter;		// Locally scaled gravitational parameter = M * G | G = gravitational constant, M = mass of local primary.
	bool							m_isInfluencing;		// Whether this scaling space is attached to the local primary.

	Vector3							m_primaryPosition;		// Locally scaled position of the primary relative to this space.
	Vector3							m_primaryVelocity;		// Locally scaled velocity of the primary relative to this space.

	SharedPtr<Particle>				m_pHost;
	std::list<SharedPtr<Particle>>	m_particles;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScalingSpace::ComputeScaledGravityParameter(float trueRadius, float hostMass)
{
	return kGravitational * hostMass * powf(trueRadius, -3.f);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool ScalingSpace::IsInfluencing() const
{
	return m_isInfluencing;
}

} // namespace Orbital ------------------------------------------------------------------------------------------------------------

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_SCALING_SPACE_H
