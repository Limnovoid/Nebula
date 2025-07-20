#ifndef NEUTRON_PARTICLE_H
#define NEUTRON_PARTICLE_H

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

class Particle
{
	friend class OrbitalSystem;

public:
	//OrbitalSystem & GetOrbitalSystem() const;
	//ScalingSphereBase * GetHostSpace() const;
	//ScalingSphereList const& GetAttachedSpaces() const;
	//ScalingSphereBase * GetSpaceOfInfluence() const;

	//float GetMass() const;
	//Vector3 const& GetPosition() const;
	//Vector3 const& GetVelocity() const;
	//bool IsInfluencing() const;
	//class Orbit const* GetOrbit() const;

private:
	Particle(class ParticleBase * pParticleBase);

	class ParticleBase *	m_pParticleBase;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_PARTICLE_H
