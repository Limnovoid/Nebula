#include "ScalingSphereBase.h"

#include "ParticleBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ScalingSphereBase::ScalingSphereBase(ParticleBase * pHostParticle, Unit::Absolute const& absoluteRadius) :
	m_pHostParticle(pHostParticle),
	m_absoluteRadius(absoluteRadius),
	m_radius(1.f),
	m_gravityParameter(0.f),
	m_pOuterSphere(nullptr),
	m_pInnerSphere(nullptr),
	m_squareRadius(0.f)
{
	assert(nullptr != m_pHostParticle);
	assert(0.f < m_absoluteRadius);
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::Initialize()
{
	if (nullptr == m_pOuterSphere)
		m_radius = Unit::Relative(1.f);
	else
		m_radius = m_absoluteRadius.ToRelative(*m_pOuterSphere);

	m_squareRadius = m_radius * m_radius;

	m_gravityParameter = ComputeScaledGravityParameter(m_absoluteRadius, GetPrimary()->GetMass());
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleBase * ScalingSphereBase::AddParticle(UniquePtr<ParticleBase> && particleBasePtr)
{
	assert(particleBasePtr->GetHostSphere() == this);

	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particleBasePtr));

	(*particleListIterator)->Initialize();

	return particleListIterator->get();
}

// --------------------------------------------------------------------------------------------------------------------------------

UniquePtr<ParticleBase> ScalingSphereBase::RemoveParticle(ParticleBase * pParticleBase)
{
	assert(pParticleBase->GetHostSphere() == this);

	ScalingSphereBase::ParticleList::iterator particleIterator = m_particles.begin();

	while (m_particles.end() != particleIterator)
	{
		if (particleIterator->get() == pParticleBase)
			break;

		++particleIterator;
	}

	assert(m_particles.end() != particleIterator);

	UniquePtr<ParticleBase> particlePtr = std::move(*particleIterator);

	m_particles.erase(particleIterator);

	return particlePtr;
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::HandleResized(Unit::Absolute const& previousAbsoluteRadius)
{
	const Unit::Relative particleRescaleFactor((previousAbsoluteRadius / m_absoluteRadius).Get());

	if (1.f == particleRescaleFactor)
		return; // No re-scaling, nothing to do.

	const bool hasRadiusIncreased = (previousAbsoluteRadius < m_absoluteRadius);

	ParticleList::iterator particleListIterator = m_particles.begin();
	while (m_particles.end() != particleListIterator)
	{
		ParticleList::iterator thisParticleIterator = particleListIterator++;

		(*thisParticleIterator)->Rescale(particleRescaleFactor);

		bool hasParticleEscaped;

		if (hasRadiusIncreased)
			hasParticleEscaped = HandleParticleMaybeEscapedToInner(thisParticleIterator);
		else // !hasTrueRadiusIncreased
			hasParticleEscaped = HandleParticleMaybeEscapedToOuter(thisParticleIterator);

		if (!hasParticleEscaped)
		{
			assert((m_particles.end() != thisParticleIterator) && (this == (*thisParticleIterator)->GetHostSphere()));
			(*thisParticleIterator)->Initialize();
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::HandleNewInnerSphere()
{
	assert(nullptr != m_pInnerSphere);

	ParticleList::iterator particleListIterator = m_particles.begin();
	while (m_particles.end() != particleListIterator)
	{
		ParticleList::iterator thisParticleIterator = particleListIterator++;

		(void) HandleParticleMaybeEscapedToInner(thisParticleIterator);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::HandleParticleSphereResized(ScalingSphereBase * pScalingSphere)
{
	assert(nullptr != pScalingSphere);
	assert(this == pScalingSphere->GetHostParticle()->GetHostSphere());
	assert(this == pScalingSphere->GetOuterSphere());

	ParticleList::iterator particleListIterator = m_particles.begin();
	while (m_particles.end() != particleListIterator)
	{
		ParticleList::iterator thisParticleIterator = particleListIterator++;

		if (thisParticleIterator->get() != pScalingSphere->GetHostParticle())
			(void) HandleParticleMaybeCaptured(thisParticleIterator, pScalingSphere);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::HandleBeingRemoved(const bool shouldDonateParticles)
{
	if (shouldDonateParticles)
	{
		if (nullptr != m_pOuterSphere)
		{
			DonateParticlesToOuter();
		}
		else
		{
			DonateParticlesToInner();
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscaped(ParticleBase * pParticle)
{
	assert(nullptr != pParticle);
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
	assert(!m_particles.empty());

	bool hasParticleEscaped = HandleParticleMaybeEscapedToInner(particleListIterator);

	if (!hasParticleEscaped)
		hasParticleEscaped = HandleParticleMaybeEscapedToOuter(particleListIterator);

	if (!hasParticleEscaped)
		hasParticleEscaped = HandleParticleMaybeCaptured(particleListIterator);

	return hasParticleEscaped;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscapedToInner(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	if (nullptr == m_pInnerSphere)
		return false;

	ParticleBase const*const pParticle = particleListIterator->get();

	const Unit::Relative particlePositionSquareMagnitude = pParticle->GetPosition().SqareMagnitude();

	if (m_pInnerSphere->GetSquareRadius() < particlePositionSquareMagnitude)
		return false;

	UniquePtr<ParticleBase> particlePtr = std::move(*particleListIterator);

	m_particles.erase(particleListIterator);

	m_pInnerSphere->ReceiveParticleFromOuter(particlePtr);

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscapedToOuter(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	if (nullptr == m_pOuterSphere)
		return false;

	ParticleBase const*const pParticle = particleListIterator->get();

	const Unit::Relative particlePositionSquareMagnitude = pParticle->GetPosition().SqareMagnitude();

	if (particlePositionSquareMagnitude < kScalingSphereEscapeRadiusSquared)
		return false;

	UniquePtr<ParticleBase> particlePtr = std::move(*particleListIterator);

	m_particles.erase(particleListIterator);

	if (m_pOuterSphere->GetHostParticle() == m_pHostParticle)
		m_pOuterSphere->ReceiveParticleFromInner(particlePtr);
	else
		m_pOuterSphere->ReceiveParticleFromEscape(particlePtr);

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeCaptured(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	for (ParticleList::iterator otherParticleListIterator = m_particles.begin(); m_particles.end() != otherParticleListIterator; ++otherParticleListIterator)
	{
		ScalingSphereBase *const pOtherParticleScalingSphere = (*otherParticleListIterator)->GetFirstSphere();

		if ((otherParticleListIterator != particleListIterator) && (nullptr != pOtherParticleScalingSphere))
		{
			if (HandleParticleMaybeCaptured(particleListIterator, pOtherParticleScalingSphere))
				return true;
		}
	}

	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeCaptured(ParticleList::iterator particleListIterator, ScalingSphereBase * pScalingSphere)
{
	assert(!m_particles.empty());
	assert((this == pScalingSphere->GetHostParticle()->GetHostSphere()) && (this == pScalingSphere->GetOuterSphere()));
	assert(pScalingSphere->GetHostParticle() != particleListIterator->get());

	const Vector3 particleSeparation = (*particleListIterator)->GetPosition() - pScalingSphere->GetHostParticle()->GetPosition();

	if (pScalingSphere->GetSquareRadius() < particleSeparation.SqareMagnitude())
		return false;

	UniquePtr<ParticleBase> particlePtr = std::move(*particleListIterator);

	m_particles.erase(particleListIterator);

	pScalingSphere->ReceiveParticleFromCapture(particlePtr);

	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromInner(UniquePtr<ParticleBase> & particlePtr, const bool shouldHandleEscape)
{
	assert(particlePtr->GetHostSphere() == m_pInnerSphere);
	assert(particlePtr->GetHostSphere()->GetOuterSphere() == this);
	assert(particlePtr->GetHostSphere()->GetHostParticle() == m_pHostParticle);

	particlePtr->Rescale(particlePtr->GetHostSphere()->GetRadius());
	particlePtr->SetHostSphere(this);

	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	if (!shouldHandleEscape)
	{
		(*particleListIterator)->Initialize();
		return;
	}

	// An ascending Particle can be anywhere outside the Sphere it has just escaped, so we check for overlaps with other Particle
	// Spheres and for escaping the outer Sphere.
	bool hasParticleEscapedAgain = HandleParticleMaybeCaptured(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToOuter(particleListIterator);

	if (!hasParticleEscapedAgain)
		(*particleListIterator)->Initialize();
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromOuter(UniquePtr<ParticleBase> & particlePtr, const bool shouldHandleEscape)
{
	assert(particlePtr->GetHostSphere() == m_pOuterSphere);
	assert(particlePtr->GetHostSphere()->GetInnerSphere() == this);
	assert(particlePtr->GetHostSphere()->GetHostParticle() == m_pHostParticle);

	particlePtr->Rescale(1.f / m_radius);
	particlePtr->SetHostSphere(this);

	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	if (!shouldHandleEscape)
	{
		(*particleListIterator)->Initialize();
		return;
	}

	// A descending Particle can be anywhere inside this Sphere, so we check for overlaps with other Particle Spheres and the
	// inner Sphere.
	bool hasParticleEscapedAgain = HandleParticleMaybeCaptured(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToInner(particleListIterator);

	if (!hasParticleEscapedAgain)
		(*particleListIterator)->Initialize();
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromEscape(UniquePtr<ParticleBase> & particlePtr, const bool shouldHandleEscape)
{
	assert(particlePtr->GetHostSphere()->GetOuterSphere() == this);
	assert(std::find_if(m_particles.begin(), m_particles.end(), [&particlePtr](UniquePtr<ParticleBase> const& otherParticlePtr) {
		return (otherParticlePtr.get() == particlePtr->GetHostSphere()->GetHostParticle()); }) != m_particles.end());

	particlePtr->Rescale(particlePtr->GetHostSphere()->GetRadius());

	particlePtr->SetPosition(particlePtr->GetPosition() + particlePtr->GetHostSphere()->GetHostParticle()->GetPosition());
	particlePtr->SetVelocity(particlePtr->GetVelocity() + particlePtr->GetHostSphere()->GetHostParticle()->GetVelocity());

	particlePtr->SetHostSphere(this);

	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	if (!shouldHandleEscape)
	{
		(*particleListIterator)->Initialize();
		return;
	}

	// An escaped Particle can now be anywhere in this Sphere except for overlapping the Sphere it has just escaped, so we check for
	// all other escape events.
	bool hasParticleEscapedAgain = HandleParticleMaybeCaptured(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToInner(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToOuter(particleListIterator);

	if (!hasParticleEscapedAgain)
		(*particleListIterator)->Initialize();
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromCapture(UniquePtr<ParticleBase> & particlePtr, const bool shouldHandleEscape)
{
	assert(m_pHostParticle->GetFirstSphere() == this);
	assert(m_pHostParticle->GetHostSphere() == particlePtr->GetHostSphere());

	particlePtr->SetPosition(particlePtr->GetPosition() - m_pHostParticle->GetPosition());
	particlePtr->SetVelocity(particlePtr->GetVelocity() - m_pHostParticle->GetVelocity());

	particlePtr->Rescale(1.f / m_radius);

	particlePtr->SetHostSphere(this);

	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	if (!shouldHandleEscape)
	{
		(*particleListIterator)->Initialize();
		return;
	}

	// A captured Particle can be anywhere inside the new host Particle's first Sphere, so we check for overlaps with the other
	// Particle Spheres and the inner Sphere.
	bool hasParticleEscapedAgain = HandleParticleMaybeCaptured(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToInner(particleListIterator);

	if (!hasParticleEscapedAgain)
		(*particleListIterator)->Initialize();
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::DonateParticlesToInner()
{
	assert(nullptr != m_pInnerSphere);

	ParticleList::iterator particleListIterator = m_particles.begin();

	while (m_particles.end() != particleListIterator)
	{
		ParticleList::iterator iteratorToErase = particleListIterator++;

		UniquePtr<ParticleBase> particlePtr = std::move(*iteratorToErase);

		m_particles.erase(iteratorToErase);

		m_pInnerSphere->ReceiveParticleFromOuter(particlePtr, false);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::DonateParticlesToOuter()
{
	assert(nullptr != m_pOuterSphere);

	ParticleList::iterator particleListIterator = m_particles.begin();

	const bool isOuterSphereAttachedToSameHost = (m_pOuterSphere->GetHostParticle() == m_pHostParticle);

	while (m_particles.end() != particleListIterator)
	{
		ParticleList::iterator iteratorToErase = particleListIterator++;

		UniquePtr<ParticleBase> particlePtr = std::move(*iteratorToErase);

		m_particles.erase(iteratorToErase);

		if (isOuterSphereAttachedToSameHost)
			m_pOuterSphere->ReceiveParticleFromInner(particlePtr, false);
		else
			m_pOuterSphere->ReceiveParticleFromEscape(particlePtr, false);
	}
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
