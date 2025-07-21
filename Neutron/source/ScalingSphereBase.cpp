#include "ScalingSphereBase.h"

#include "ParticleBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ScalingSphereBase::ScalingSphereBase(ParticleBase * pHostParticle, float trueRadius) :
	m_pHostParticle(pHostParticle),
	m_pOuterSpace(nullptr),
	m_pInnerSpace(nullptr),
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
	if (nullptr == m_pOuterSpace)
		m_radius = 1.f;
	else
		m_radius = m_trueRadius / m_pOuterSpace->m_trueRadius;

	m_gravityParameter = ComputeScaledGravityParameter(m_trueRadius, GetPrimary()->GetMass());
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::HandleResized(const float previousTrueRadius)
{
	const float particleRescaleFactor = previousTrueRadius / m_trueRadius;

	for (ParticleList::iterator particleListIter = m_particles.begin(); m_particles.end() != particleListIter; ++particleListIter)
	{
		(*particleListIter)->Rescale(particleRescaleFactor);

		ScalingSphereBase * pNewScalingSphere = nullptr;
		if (previousTrueRadius < m_trueRadius) // Radius increased ...
		{
			pNewScalingSphere = HandleParticleMaybeEscapedToInner(particleListIter);
		}
		else if (m_trueRadius < previousTrueRadius) // Radius decreased ...
		{
			pNewScalingSphere = HandleParticleMaybeEscapedToOuter(particleListIter);
		}

		// Explicitly re-initialize particles which have NOT escaped.
		if (this == pNewScalingSphere)
			(*particleListIter)->Initialize();
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * ScalingSphereBase::HandleParticleMaybeEscaped(ParticleBase * pParticle)
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
		return nullptr;

	return HandleParticleMaybeEscaped(particleListIterator);
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * ScalingSphereBase::HandleParticleMaybeEscaped(ParticleList::iterator particleListIterator)
{
	assert(false); // TODO - check if escaped into outer Sphere

	return this;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * ScalingSphereBase::HandleParticleMaybeEscapedToInner(ParticleList::iterator particleListIterator)
{
	return this;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * ScalingSphereBase::HandleParticleMaybeEscapedToOuter(ParticleList::iterator particleListIterator)
{
	return this;
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
