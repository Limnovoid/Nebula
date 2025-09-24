#ifndef NEUTRON_I_SCALED_SPACE_H
#define NEUTRON_I_SCALED_SPACE_H

#include "Constants.h"
#include "Uuid.h"
#include "Types.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class ParticleBase;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ScalingSphereBase
{
	friend class ScalingSphereList;

public:
	using ParticleList = std::list<UniquePtr<ParticleBase>>;

	/// <summary> Compute the scaled gravitational parameter of a primary with given mass. </summary>
	/// <param name="absoluteRadius"> The absolute radius of the scaling sphere whose gravitational parameter is being computed. </param>
	/// <param name="primaryMass"> The mass of the scaling sphere's primary. </param>
	/// <returns> The scaled gravitational parameter. </returns>
	static Unit::Relative ComputeScaledGravityParameter(Unit::Absolute const& absoluteRadius, float primaryMass);

	ScalingSphereBase(ParticleBase * pHostParticle, Unit::Absolute const& absoluteRadius);
	virtual ~ScalingSphereBase() = default;

	void SetAbsoluteRadius(const Unit::Absolute const& absoluteRadius);

	virtual void Initialize();

	ParticleBase * GetHostParticle() const;
	ParticleList const& GetParticleList() const;
	ScalingSphereBase * GetOuterSphere() const;
	ScalingSphereBase * GetInnerSphere() const;

	Unit::Absolute GetAbsoluteRadius() const;
	Unit::Relative GetRadius() const;
	Unit::Relative GetSquareRadius() const;
	Unit::Relative GetGravityParameter() const;

	virtual bool IsInfluencing() const = 0;
	virtual ParticleBase const* GetPrimary() const = 0;
	virtual RelVector3 const& GetPrimaryPosition() const = 0;
	virtual RelVector3 const& GetPrimaryVelocity() const = 0;

	ParticleBase * AddParticle(UniquePtr<ParticleBase> && particleBasePtr);
	UniquePtr<ParticleBase> RemoveParticle(ParticleBase * pParticleBase);

	Unit::Relative CircularOrbitSpeed(Unit::Relative const& orbitRadius) const;

	void HandleResized(Unit::Absolute const& previousAbsoluteRadius);
	void HandleNewInnerSphere();
	void HandleParticleSphereResized(ScalingSphereBase * pScalingSphere);
	void HandleBeingRemoved(const bool shouldDonateParticles);

	/// <summary> If the given Particle has escaped this Sphere, transfer ownership to the appropriate Sphere. </summary>
	/// <param name="pParticle"> The Particle which may have escaped this Sphere. </param>
	/// <returns> True if the particle escaped, otherwise false. NOTE: Returns false if the particle does not belong to this Sphere. </returns>
	bool HandleParticleMaybeEscaped(ParticleBase * pParticle);

	Uuid						m_uuid;

protected:
	// Family of functions to handle checking Particle positions and transferring their ownership to adjacent Spheres as required.
	bool HandleParticleMaybeEscaped(ParticleList::iterator particleListIterator);
	bool HandleParticleMaybeEscapedToInner(ParticleList::iterator particleListIterator);
	bool HandleParticleMaybeEscapedToOuter(ParticleList::iterator particleListIterator);
	bool HandleParticleMaybeCaptured(ParticleList::iterator particleListIterator);
	bool HandleParticleMaybeCaptured(ParticleList::iterator particleListIterator, ScalingSphereBase * pParticleScalingSphere);

	// Family of functions to receive ownership of Particles from adjacent Spheres.
	void ReceiveParticleFromInner(UniquePtr<ParticleBase> & particlePtr, const bool shouldHandleEscape = true);		// Receive Particle ascending from the inner Sphere.
	void ReceiveParticleFromOuter(UniquePtr<ParticleBase> & particlePtr, const bool shouldHandleEscape = true);		// Receive Particle descending from the outer Sphere on the same host Particle.
	void ReceiveParticleFromEscape(UniquePtr<ParticleBase> & particlePtr, const bool shouldHandleEscape = true);	// Receive Particle leaving the first Sphere of another Particle in this Sphere.
	void ReceiveParticleFromCapture(UniquePtr<ParticleBase> & particlePtr, const bool shouldHandleEscape = true);	// Receive Particle entering this Sphere from the host Particle's host Sphere.

	void DonateParticlesToInner();
	void DonateParticlesToOuter();

	ParticleBase *			m_pHostParticle;
	ParticleList			m_particles;

	Unit::Absolute		m_absoluteRadius;		// Radius in meters.
	Unit::Relative		m_radius;				// Radius relative to outer sphere.
	Unit::Relative		m_squareRadius;			// Square of relative radius.
	Unit::Relative		m_gravityParameter;		// Locally scaled gravitational parameter = M * G / r^3 | G = gravitational constant, M = mass of local primary, r = true radius.

private:
	ScalingSphereBase *		m_pOuterSphere;
	ScalingSphereBase *		m_pInnerSphere;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline Unit::Relative ScalingSphereBase::ComputeScaledGravityParameter(Unit::Absolute const& absoluteRadius, float primaryMass)
{
	// Gravity parameter / (true radius)^3 = G * M / r^3.
	return Unit::Relative( kGravitational * primaryMass * powf(absoluteRadius.Get(), -3.f) );
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void ScalingSphereBase::SetAbsoluteRadius(Unit::Absolute const& absoluteRadius)
{
	m_absoluteRadius = absoluteRadius;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ParticleBase * ScalingSphereBase::GetHostParticle() const
{
	return m_pHostParticle;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereBase::ParticleList const& ScalingSphereBase::GetParticleList() const
{
	return m_particles;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereBase * ScalingSphereBase::GetOuterSphere() const
{
	return m_pOuterSphere;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereBase * ScalingSphereBase::GetInnerSphere() const
{
	return m_pInnerSphere;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Unit::Absolute ScalingSphereBase::GetAbsoluteRadius() const
{
	return m_absoluteRadius;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Unit::Relative ScalingSphereBase::GetRadius() const
{
	return m_radius;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Unit::Relative ScalingSphereBase::GetSquareRadius() const
{
	return m_squareRadius;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Unit::Relative ScalingSphereBase::GetGravityParameter() const
{
	return m_gravityParameter;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Unit::Relative ScalingSphereBase::CircularOrbitSpeed(Unit::Relative const& orbitRadius) const
{
	// Velocity magnitude of a circular orbit = sqrt(gravity parameter / orbit radius).
	return Unit::Relative(sqrtf(( m_gravityParameter / orbitRadius ).Get()));
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_I_SCALED_SPACE_H
