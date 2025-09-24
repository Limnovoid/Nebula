#ifndef NEUTRON_I_PARTICLE_H
#define NEUTRON_I_PARTICLE_H

#include "NebulaTypes.h"
#include "ScalingSphereList.h"
#include "Uuid.h"
#include "Types.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;
using namespace Nova;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ParticleBase
{
public:
	/// <summary> Compute the radius of influence for an influencing particle. </summary>
	/// <param name="orbitRadius"> The orbit radius of the influencing particle (relative or absolute). </param>
	/// <param name="particleMass"> The mass of the influencing particle. </param>
	/// <param name="primaryMass"> The mass of the primary particle. </param>
	/// <returns> The radius of influence (relative or absolute, the same as the orbit radius). </returns>
	static float ComputeRadiusOfInfluence(float orbitRadius, float particleMass, float primaryMass);

	ParticleBase(ScalingSphereBase * pHostSphere, float mass);
	virtual ~ParticleBase();

	void SetHostSphere(ScalingSphereBase * pHostSphere);
	void SetMass(const float mass);

	virtual void SetPosition(RelVector3 const& position) = 0;
	virtual void SetVelocity(RelVector3 const& velocity) = 0;
	virtual void Rescale(Unit::Relative const& rescaleFactor) = 0;
	virtual void Initialize() = 0;

	virtual bool IsInfluencing() const = 0;
	virtual ScalingSphereBase * GetSphereOfInfluence() const = 0;
	virtual RelVector3 const& GetPosition() const = 0;
	virtual RelVector3 const& GetVelocity() const = 0;
	virtual class Orbit const* GetOrbit() const = 0;

	ScalingSphereBase * AddScalingSphere(UniquePtr<ScalingSphereBase> && scalingSphereBasePtr);
	UniquePtr<ScalingSphereBase> RemoveScalingSphere(ScalingSphereBase * pScalingSphereBase, const bool shouldDonateParticles);
	Result ResizeScalingSphere(ScalingSphereBase * pScalingSphereBase, Unit::Absolute const& absoluteRadius);
	Result ResizeSphereOfInfluence(Unit::Absolute const& absoluteRadius);

	ScalingSphereBase * GetHostSphere() const;
	ScalingSphereBase * GetFirstSphere() const;
	ScalingSphereList const& GetScalingSphereList() const;
	float GetMass() const;

	Uuid						m_uuid;

private:
	Result ResizeScalingSphereImpl(ScalingSphereBase * pScalingSphereBase, Unit::Absolute const& absoluteRadius);

	ScalingSphereBase *			m_pHostSphere;		// Pointer to the scaling spheres in which this particle is moving, or the orbital system's host space if this particle is the system host particle.
	ScalingSphereList			m_attachedSpheres;	// List of pointers to scaling spheres attached to this particle.

	float						m_mass;				// The particle mass.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline float ParticleBase::ComputeRadiusOfInfluence(float orbitRadius, float particleMass, float primaryMass)
{
	return orbitRadius * powf(particleMass / primaryMass, 2.f / 5.f);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void ParticleBase::SetHostSphere(ScalingSphereBase * pHostSphere)
{
	m_pHostSphere = pHostSphere;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void ParticleBase::SetMass(const float mass)
{
	m_mass = mass;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereBase * ParticleBase::GetHostSphere() const
{
	return m_pHostSphere;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScalingSphereList const& ParticleBase::GetScalingSphereList() const
{
	return m_attachedSpheres;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ParticleBase::GetMass() const
{
	return m_mass;
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_I_PARTICLE_H
