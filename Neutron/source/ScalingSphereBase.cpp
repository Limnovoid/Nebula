#include "ScalingSphereBase.h"

#include "ParticleBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ScalingSphereBase::ScalingSphereBase(ParticleBase * pHostParticle, float trueRadius) :
	m_pHostParticle(pHostParticle),
	m_trueRadius(trueRadius),
	m_radius(1.f),
	m_gravityParameter(0.f),
	m_pOuterSphere(nullptr),
	m_pInnerSphere(nullptr),
	m_squareRadius(0.f)
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

	m_squareRadius = m_radius * m_radius;

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

void ScalingSphereBase::HandleParticleSphereResized(ScalingSphereBase * pScalingSphere)
{
	assert(nullptr != pScalingSphere);
	assert(this == pScalingSphere->GetHostParticle()->GetHostSphere());
	assert(this == pScalingSphere->m_pOuterSphere);

	for (ParticleList::iterator particleListIter = m_particles.begin(); m_particles.end() != particleListIter; ++particleListIter)
	{
		if (particleListIter->get() != pScalingSphere->GetHostParticle())
			(void) HandleParticleMaybeEscaped(particleListIter, pScalingSphere);
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

void ScalingSphereBase::ReceiveParticleFromInner(UniquePtr<ParticleBase> && particlePtr)
{
	assert((this == particlePtr->GetHostSphere()->GetOuterSphere()) && (particlePtr->GetHostSphere()->GetHostParticle() == m_pHostParticle));

	assert(false); // TODO - re-position and re-scale for this Sphere



	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	if (!HandleParticleMaybeEscapedToOuter(particleListIterator))
		(void) HandleParticleMaybeEscapedToParticleSphere(particleListIterator);
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromOuter(UniquePtr<ParticleBase> && particlePtr)
{
	assert(this == particlePtr->GetHostSphere()->GetInnerSphere());

	assert(false); // TODO - re-position and re-scale for this Sphere
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromEscape(UniquePtr<ParticleBase> && particlePtr)
{
	assert(false); // TODO - re-position and re-scale for this Sphere
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromCapture(UniquePtr<ParticleBase> && particlePtr)
{
	assert(this == particlePtr->GetHostSphere()->m_pOuterSphere);
	assert(this == particlePtr->GetHostSphere()->GetHostParticle()->GetHostSphere());

	assert(false); // TODO - re-position and re-scale for this Sphere

	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	if (!HandleParticleMaybeEscapedToOuter(particleListIterator))
		(void)HandleParticleMaybeEscapedToParticleSphere(particleListIterator);
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscaped(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	assert(false); // TODO - check if escaped this Sphere

	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscaped(ParticleList::iterator particleListIterator, ScalingSphereBase * pScalingSphere)
{
	assert(!m_particles.empty());

	assert(false); // TODO - check if escaped into referenced Sphere

	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscapedToInner(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	assert(false); // TODO - check if escaped into outer Sphere

	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscapedToOuter(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	ParticleBase * pParticle = particleListIterator->get();

	const float particlePositionSquareMagnitude = pParticle->GetPosition().SqareMagnitude();
	assert(nullptr == m_pInnerSphere ? (0.f < particlePositionSquareMagnitude) : (m_pInnerSphere->m_radius < particlePositionSquareMagnitude));

	if (particlePositionSquareMagnitude < kScalingSphereEscapeRadius)
		return false;

	UniquePtr<ParticleBase> particlePtr = std::move(*particleListIterator);

	m_particles.erase(particleListIterator);

	if (m_pOuterSphere->GetHostParticle() == m_pHostParticle)
		m_pOuterSphere->ReceiveParticleFromInner(std::move(particlePtr));
	else
		m_pOuterSphere->ReceiveParticleFromEscape(std::move(particlePtr));

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscapedToParticleSphere(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	ParticleList::iterator otherParticleListIterator = m_particles.begin();

	do
	{
		ScalingSphereBase *const pOtherParticleScalingSphere = (*otherParticleListIterator)->GetFirstSphere();

		if ((otherParticleListIterator != particleListIterator) && (nullptr != pOtherParticleScalingSphere))
		{
			const Vector3 otherParticleSeparation = (*particleListIterator)->GetPosition() - (*otherParticleListIterator)->GetPosition();

			if (otherParticleSeparation.SqareMagnitude() <= pOtherParticleScalingSphere->GetSquareRadius())
				break;
		}

		++particleListIterator;
	}
	while (m_particles.end() != otherParticleListIterator);

	if (m_particles.end() == particleListIterator)
		return false;

	return HandleParticleMaybeEscaped(particleListIterator);
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
