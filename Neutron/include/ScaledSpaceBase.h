#ifndef NEUTRON_I_SCALED_SPACE_H
#define NEUTRON_I_SCALED_SPACE_H

#include "Vector3.h"
#include "Constants.h"
#include "Uuid.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class ScaledSpaceBase
{
	friend class OrbitalSystem2;
	friend class ParticleBase;
	friend class ScaledSpaceList;

public:
	/// <summary> Compute the scaled gravitational parameter of a primary with given mass. </summary>
	/// <param name="trueRadius"> The true radius of the scaled space whose gravitational parameter is being computed. </param>
	/// <param name="primaryMass"> The mass of the scaled space's primary. </param>
	/// <returns> The scaled gravitational parameter. </returns>
	static float ComputeScaledGravityParameter(float trueRadius, float primaryMass);

	ScaledSpaceBase(ParticleBase * pHostParticle, float trueRadius);
	virtual ~ScaledSpaceBase() = default;

	virtual void Initialize(float radius);

	ParticleBase * GetHostParticle() const;
	float GetTrueRadius() const;
	float GetRadius() const;
	float GetGravityParameter() const;
	float CircularOrbitSpeed(float orbitRadius) const;

	/// <summary> Modify the radius. Re-intializes the scaled space with the new radius. </summary>
	/// <param name="radius"> The new radius, relative to the containing space. </param>
	/// <exception cref="ApiException"> Invalid parameter or state. </exception>
	void SetRadius(float radius);

	virtual bool IsInfluencing() const = 0;
	virtual ParticleBase const& GetPrimary() const = 0;
	virtual Vector3 const& GetPrimaryPosition() const = 0;
	virtual Vector3 const& GetPrimaryVelocity() const = 0;

	Uuid									m_uuid;

protected:
	ParticleBase *						m_pHostParticle;
	std::list<UniquePtr<ParticleBase>>	m_particles;

	float								m_trueRadius;			// Radius in meters.
	float								m_radius;				// Radius relative to superior scaling space.
	float								m_gravityParameter;		// Locally scaled gravitational parameter = M * G / r^3 | G = gravitational constant, M = mass of local primary, r = true radius.

private:
	ScaledSpaceBase *					m_pOuterSpace;
	ScaledSpaceBase *					m_pInnerSpace;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScaledSpaceBase::ComputeScaledGravityParameter(float trueRadius, float primaryMass)
{
	// Gravity parameter / (true radius)^3 = G * M / r^3.
	return kGravitational * primaryMass * powf(trueRadius, -3.f);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase * ScaledSpaceBase::GetHostParticle() const
{
	return m_pHostParticle;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScaledSpaceBase::GetTrueRadius() const
{
	return m_trueRadius;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScaledSpaceBase::GetRadius() const
{
	return m_radius;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScaledSpaceBase::GetGravityParameter() const
{
	return m_gravityParameter;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ScaledSpaceBase::CircularOrbitSpeed(float orbitRadius) const
{
	// Magnitude of velocity of a circular orbit = sqrt(gravity parameter / orbit radius).
	return sqrtf(m_gravityParameter / orbitRadius);
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_I_SCALED_SPACE_H
