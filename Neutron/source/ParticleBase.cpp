#include "ParticleBase.h"

#include "ScaledSpaceBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ParticleBase::ParticleBase(ScaledSpaceBase * pHostSpace, float mass) :
	m_pHostSpace(pHostSpace),
	m_mass(mass)
{
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
