#ifndef NEUTRON_I_PARTICLE_H
#define NEUTRON_I_PARTICLE_H

#include "NebulaTypes.h"
#include "ScaledSpaceList.h"
#include "Vector3.h"
#include "Uuid.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;
using namespace Nova;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ParticleBase
{
	friend class OrbitalSystem2;

public:
	static float ComputeRadiusOfInfluence(float orbitRadius, float particleMass, float primaryMass);

	ParticleBase(OrbitalSystem2 & orbitalSystem, ScaledSpaceBase * pHostSpace, float mass);
	virtual ~ParticleBase();

	OrbitalSystem2 & GetOrbitalSystem();
	ScaledSpaceBase * GetHostSpace();
	ScaledSpaceList & GetAttachedSpaces();
	float GetMass() const;

	virtual Vector3 const& GetPosition() const = 0;
	virtual Vector3 const& GetVelocity() const = 0;
	virtual bool IsInfluencing() const = 0;
	virtual ScaledSpaceBase * GetSpaceOfInfluence() const = 0;
	virtual class Orbit const* GetOrbit() const = 0;

	Uuid									m_uuid;

protected:
	template<typename TScaledSpace>
	TScaledSpace * EmplaceScaledSpace(float trueRadius);

	OrbitalSystem2 &						m_orbitalSystem;				// Reference to the orbital system.

	ScaledSpaceBase *						m_pHostSpace;					// Pointer to the scaling space in which this particle is moving, or the orbital system's host space if this particle is the system host particle.
	ScaledSpaceList							m_attachedSpaces;				// List of pointers to scaling spaces attached to this particle.

	float									m_mass;							// The particle mass.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline float ParticleBase::ComputeRadiusOfInfluence(float orbitRadius, float particleMass, float primaryMass)
{
	return orbitRadius * powf(particleMass / primaryMass, 2.f / 5.f);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline OrbitalSystem2 & ParticleBase::GetOrbitalSystem()
{
	return m_orbitalSystem;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScaledSpaceBase * ParticleBase::GetHostSpace()
{
	return m_pHostSpace;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScaledSpaceList & ParticleBase::GetAttachedSpaces()
{
	return m_attachedSpaces;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline float ParticleBase::GetMass() const
{
	return m_mass;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TScaledSpace>
inline TScaledSpace * ParticleBase::EmplaceScaledSpace(float trueRadius)
{
	return static_cast<TScaledSpace *>(m_attachedSpaces.Emplace<TScaledSpace>(this, trueRadius)->get());
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_I_PARTICLE_H
