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

		bool hasParticleEscaped;

		if (previousTrueRadius < m_trueRadius)
			hasParticleEscaped = HandleParticleMaybeEscapedToInner(particleListIter);
		else // m_trueRadius < previousTrueRadius
			hasParticleEscaped = HandleParticleMaybeEscapedToOuter(particleListIter);

		if (!hasParticleEscaped)
		{
			assert((m_particles.end() != particleListIter) && (this == (*particleListIter)->GetHostSphere()));
			(*particleListIter)->Initialize();
		}
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
	assert(this == pScalingSphere->GetOuterSphere());

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
	assert(this == particlePtr->GetHostSphere()->GetOuterSphere());
	assert(particlePtr->GetHostSphere()->GetHostParticle() == m_pHostParticle);

	particlePtr->Rescale(particlePtr->GetHostSphere()->GetRadius());
	particlePtr->SetHostSphere(this);

	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	// A descending Particle can be anywhere inside this Sphere, so we check for overlaps with other Particle Spheres and the
	// inner Sphere.
	bool hasParticleEscapedAgain = HandleParticleMaybeEscapedToParticleSphere(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToOuter(particleListIterator);

	if (!hasParticleEscapedAgain)
		(*particleListIterator)->Initialize();
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromOuter(UniquePtr<ParticleBase> && particlePtr)
{
	assert(this == particlePtr->GetHostSphere()->GetInnerSphere());
	assert(particlePtr->GetHostSphere()->GetHostParticle() == m_pHostParticle);

	particlePtr->Rescale(1.f / m_radius);
	particlePtr->SetHostSphere(this);

	// An ascending Particle can be anywhere outside the Sphere it has just escaped, so we check for overlaps with other Particle
	// Spheres and for escaping the outer Sphere.
	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	bool hasParticleEscapedAgain = HandleParticleMaybeEscapedToParticleSphere(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToInner(particleListIterator);

	if (!hasParticleEscapedAgain)
		(*particleListIterator)->Initialize();
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromEscape(UniquePtr<ParticleBase> && particlePtr)
{
	assert(this == particlePtr->GetHostSphere()->GetOuterSphere());
	assert(particlePtr->GetHostSphere()->GetHostParticle() != m_pHostParticle);

	particlePtr->Rescale(particlePtr->GetHostSphere()->GetRadius());
	particlePtr->SetPosition(particlePtr->GetPosition() + particlePtr->GetHostSphere()->GetHostParticle()->GetPosition());
	particlePtr->SetVelocity(particlePtr->GetVelocity() + particlePtr->GetHostSphere()->GetHostParticle()->GetVelocity());

	particlePtr->SetHostSphere(this);

	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	// An escaped Particle can now be anywhere in this Sphere except for overlapping the Sphere it has just escaped, so we check for
	// all other escape events.
	bool hasParticleEscapedAgain = HandleParticleMaybeEscapedToParticleSphere(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToInner(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToOuter(particleListIterator);

	if (!hasParticleEscapedAgain)
		(*particleListIterator)->Initialize();
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereBase::ReceiveParticleFromCapture(UniquePtr<ParticleBase> && particlePtr)
{
	assert(particlePtr->GetHostSphere() == m_pHostParticle->GetHostSphere());

	particlePtr->SetPosition(particlePtr->GetPosition() - m_pHostParticle->GetPosition());
	particlePtr->SetVelocity(particlePtr->GetVelocity() - m_pHostParticle->GetVelocity());
	particlePtr->Rescale(1.f / m_radius);

	particlePtr->SetHostSphere(this);

	// A captured Particle can be anywhere inside the new host Particle's first Sphere, so we check for overlaps with the other
	// Particle spheres and the inner Sphere.
	const ParticleList::iterator particleListIterator = m_particles.insert(m_particles.end(), std::move(particlePtr));

	bool hasParticleEscapedAgain = HandleParticleMaybeEscapedToParticleSphere(particleListIterator);

	if (!hasParticleEscapedAgain)
		hasParticleEscapedAgain = HandleParticleMaybeEscapedToInner(particleListIterator);

	if (!hasParticleEscapedAgain)
		(*particleListIterator)->Initialize();
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscaped(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	bool hasParticleEscaped = HandleParticleMaybeEscapedToInner(particleListIterator);

	if (!hasParticleEscaped)
		hasParticleEscaped = HandleParticleMaybeEscapedToOuter(particleListIterator);

	if (!hasParticleEscaped)
		hasParticleEscaped = HandleParticleMaybeEscapedToParticleSphere(particleListIterator);

	return hasParticleEscaped;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscaped(ParticleList::iterator particleListIterator, ScalingSphereBase * pScalingSphere)
{
	assert(!m_particles.empty());
	assert(this == pScalingSphere->GetHostParticle()->GetHostSphere());

	assert(false); // TODO - check if escaped into referenced Sphere

	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscapedToInner(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	assert(false); // TODO - check if escaped into inner Sphere

	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereBase::HandleParticleMaybeEscapedToOuter(ParticleList::iterator particleListIterator)
{
	assert(!m_particles.empty());

	ParticleBase const*const pParticle = particleListIterator->get();

	const float particlePositionSquareMagnitude = pParticle->GetPosition().SqareMagnitude();
	assert((nullptr == m_pInnerSphere) ? (0.f < particlePositionSquareMagnitude) : (m_pInnerSphere->GetSquareRadius() < particlePositionSquareMagnitude));

	if (particlePositionSquareMagnitude < kScalingSphereEscapeRadiusSquared)
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

	if (m_particles.end() == otherParticleListIterator)
		return false;

	return HandleParticleMaybeEscaped(particleListIterator, (*otherParticleListIterator)->GetFirstSphere());
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
