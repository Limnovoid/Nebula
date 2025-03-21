#ifndef NEUTRON_SCALING_SPACE_H
#define NEUTRON_SCALING_SPACE_H

#include "Vector3.h"
#include "Constants.h"
#include "Maths.h"
#include "Uuid.h"
#include "SortedList.h"
#include "ITestScript.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;
using namespace Nova;

class Particle;

// --------------------------------------------------------------------------------------------------------------------------------

class ScalingSpace
{
	friend class OrbitalSystem;
	friend class OrbitalSystemTestScript;

public:
	/// <summary> Compute the locally scaled gravitational parameter of the local primary. </summary>
	/// <param name="trueRadius"></param>
	/// <param name="primaryMass"></param>
	/// <returns></returns>
	static float ComputeScaledGravityParameter(float trueRadius, float primaryMass);

	/// <summary> Compute the position and velocity of the local primary relative/scaled to this scaling space. </summary>
	static void ComputePrimaryKinetics(ScalingSpace const* pScalingSpace, Vector3 & position, Vector3 & velocity);

	ScalingSpace(Particle * pHost, float radius, float trueRadius, bool isInfluencing);

	float GetTrueRadius() const;
	float GetRadius() const;
	bool IsInfluencing() const;
	Particle const& GetPrimary() const;
	Vector3 const& GetPrimaryPosition() const;
	Vector3 const& GetPrimaryVelocity() const;
	float GetGravityParameter() const;

	float CircularOrbitSpeed(float orbitRadius);

	Uuid								m_uuid;

private:
	/// <summary> Search up through the simulation tree for this scaling space's primary. </summary>
	/// <param name="pScalingSpace"> The scaling space whose primary to search for. </param>
	/// <returns> A pointer to the primary. </returns>
	static Particle * FindPrimary(ScalingSpace const* pScalingSpace);

	float							m_trueRadius;			// Radius in meters.
	float							m_radius;				// Radius relative to superior scaling space.

	float							m_gravityParameter;		// Locally scaled gravitational parameter = M * G / r^3 | G = gravitational constant, M = mass of local primary, r = true radius.
	bool							m_isInfluencing;		// Whether this scaling space is attached to the local primary.

	Particle *						m_pPrimary;				// Pointer to the local primary.
	Vector3							m_primaryPosition;		// Locally scaled position of the primary relative to this space.
	Vector3							m_primaryVelocity;		// Locally scaled velocity of the primary relative to this space.

	Particle *						m_pHost;
	std::list<UniquePtr<Particle>>	m_particles;
	ScalingSpace *					m_pOuterSpace;
	ScalingSpace *					m_pInnerSpace;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScalingSpace::ComputeScaledGravityParameter(float trueRadius, float primaryMass)
{
	// Gravity parameter / (true radius)^3 = G * M / r^3.
	return kGravitational * primaryMass * powf(trueRadius, -3.f);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScalingSpace::GetTrueRadius() const
{
	return m_trueRadius;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScalingSpace::GetRadius() const
{
	return m_radius;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool ScalingSpace::IsInfluencing() const
{
	return m_isInfluencing;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Particle const& ScalingSpace::GetPrimary() const
{
	return *m_pPrimary;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& ScalingSpace::GetPrimaryPosition() const
{
	return m_primaryPosition;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Vector3 const& ScalingSpace::GetPrimaryVelocity() const
{
	return m_primaryVelocity;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScalingSpace::GetGravityParameter() const
{
	return m_gravityParameter;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScalingSpace::CircularOrbitSpeed(float orbitRadius)
{
	// Magnitude of velocity of a circular orbit = sqrt(gravity parameter / orbit radius).
	return sqrtf(m_gravityParameter / orbitRadius);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ScalingSpaceTestScript : public ITestScript
{
public:
	ScalingSpaceTestScript();
	virtual ~ScalingSpaceTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_SCALING_SPACE_H
