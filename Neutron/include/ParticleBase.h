#ifndef NEUTRON_I_PARTICLE_H
#define NEUTRON_I_PARTICLE_H

#include "NebulaTypes.h"
#include "ScalingSphereList.h"
#include "Vector3.h"
#include "Uuid.h"
#include "NeedsInitializationHelper.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;
using namespace Nova;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ParticleBase
{
public:
	static float ComputeRadiusOfInfluence(float orbitRadius, float particleMass, float primaryMass);

	ParticleBase(ScalingSphereBase * pHostSphere, float mass);
	virtual ~ParticleBase();

	void SetHostSphere(ScalingSphereBase * pHostSphere);
	void SetMass(const float mass);

	virtual void SetPosition(Vector3 const& position) = 0;
	virtual void SetVelocity(Vector3 const& velocity) = 0;
	virtual void Rescale(const float rescaleFactor) = 0;
	virtual void Initialize() = 0;

	ScalingSphereBase * AddScalingSphere(UniquePtr<ScalingSphereBase> &&scalingSphereBasePtr);
	UniquePtr<ScalingSphereBase> RemoveScalingSphere(ScalingSphereBase * pScalingSphereBase);
	Result ResizeScalingSphere(ScalingSphereBase * pScalingSphereBase, const float trueRadius);

	ScalingSphereBase * GetHostSphere() const;
	ScalingSphereBase * GetFirstSphere() const;
	ScalingSphereList const& GetScalingSphereList() const;
	float GetMass() const;

	virtual bool IsInfluencing() const = 0;
	virtual ScalingSphereBase * GetSphereOfInfluence() const = 0;
	virtual Vector3 const& GetPosition() const = 0;
	virtual Vector3 const& GetVelocity() const = 0;
	virtual class Orbit const* GetOrbit() const = 0;

	Uuid						m_uuid;
	NeedsInitializationHelper	m_needsInitializationHelper;

protected:
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

	m_needsInitializationHelper.Set();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void ParticleBase::SetMass(const float mass)
{
	m_mass = mass;

	m_needsInitializationHelper.Set();
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
