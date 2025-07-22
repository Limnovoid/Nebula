#include "ScalingSphereBase.h"

#include "ParticleBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ScalingSphereBase::ScalingSphereBase(ParticleBase * pHostParticle, float trueRadius) :
	m_pHostParticle(pHostParticle),
	m_pOuterSphere(nullptr),
	m_pInnerSphere(nullptr),
	m_trueRadius(trueRadius),
	m_radius(1.f),
	m_gravityParameter(0.f)
{
	assert(nullptr != m_pHostParticle);
	assert(0.f < m_trueRadius);
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::Initialize()
{
	if (nullptr == m_pOuterSphere)
		m_radius = 1.f;
	else
		m_radius = m_trueRadius / m_pOuterSphere->m_trueRadius;

	m_gravityParameter = ComputeScaledGravityParameter(m_trueRadius, GetPrimary()->GetMass());
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::HandleResized(const float previousTrueRadius)
{
	const float particleRescaleFactor = previousTrueRadius / m_trueRadius;

	if (1.f == particleRescaleFactor)
		return; // No re-scaling, nothing to do.

	for (ParticleList::iterator particleListIter = m_particles.begin(); m_particles.end() != particleListIter; ++particleListIter)
	{
		(*particleListIter)->Rescale(particleRescaleFactor);

		bool hasEscaped = false;

		if (previousTrueRadius < m_trueRadius)
			hasEscaped = HandleParticleMaybeEscapedToInner(particleListIter);
		else // m_trueRadius < previousTrueRadius
			hasEscaped = HandleParticleMaybeEscapedToOuter(particleListIter);

		if (!hasEscaped)
			(*particleListIter)->Initialize();
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::HandleNewInnerSphere()
{
	assert(nullptr != m_pInnerSphere);

	for (ParticleList::iterator particleListIter = m_particles.begin(); m_particles.end() != particleListIter; ++particleListIter)
	{
		(void) HandleParticleMaybeEscapedToInner(particleListIter);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscaped(ParticleBase * pParticle)
{
	assert(this == pParticle->GetHostSphere());

	ParticleList::iterator particleListIterator = m_particles.begin();
	
	while (m_particles.end() != particleListIterator)
	{
		if (particleListIterator->get() == pParticle)
			break;

		++particleListIterator;
	};

	if (m_particles.end() == particleListIterator)
		return false;

	return HandleParticleMaybeEscaped(particleListIterator);
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscaped(ParticleList::iterator particleListIterator)
{
	assert(false); // TODO - check if escaped into outer Sphere

	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscapedToInner(ParticleList::iterator particleListIterator)
{
	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscapedToOuter(ParticleList::iterator particleListIterator)
{
	return false;
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
