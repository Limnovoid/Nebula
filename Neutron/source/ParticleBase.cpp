#include "ParticleBase.h"

#include "OrbitalSystem2.h"
#include "ScaledSpaceBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ParticleBase::ParticleBase(OrbitalSystem2 & orbitalSystem, ScaledSpaceBase * pHostSpace, float mass) :
	m_orbitalSystem(orbitalSystem),
	m_pHostSpace(pHostSpace),
	m_mass(mass)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleBase::~ParticleBase()
{
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
