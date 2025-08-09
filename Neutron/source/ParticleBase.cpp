#include "ParticleBase.h"

#include "ScalingSphereBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

ParticleBase::ParticleBase(ScalingSphereBase * pHostSphere, float mass) :
	m_pHostSphere(pHostSphere),
	m_mass(mass)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ParticleBase::~ParticleBase()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * ParticleBase::AddScalingSphere(UniquePtr<ScalingSphereBase> && scalingSphereBasePtr)
{
	assert(scalingSphereBasePtr->GetHostParticle() == this);

	const ScalingSphereList::Iterator scalingSphereIterator = m_attachedSpheres.Insert(std::forward<UniquePtr<ScalingSphereBase>>(scalingSphereBasePtr));

	(*scalingSphereIterator)->Initialize();

	return scalingSphereIterator->get();
}

// --------------------------------------------------------------------------------------------------------------------------------

UniquePtr<ScalingSphereBase> ParticleBase::RemoveScalingSphere(ScalingSphereBase * pScalingSphereBase, const bool shouldDonateParticles)
{
	assert(pScalingSphereBase->GetHostParticle() == this);

	ScalingSphereList::Iterator scalingSphereIter = m_attachedSpheres.Find(pScalingSphereBase);

	if (m_attachedSpheres.End() == scalingSphereIter)
		return nullptr;

	pScalingSphereBase->HandleBeingRemoved(shouldDonateParticles);

	ScalingSphereBase *const pOuterSphereOnHostParticle = (m_attachedSpheres.Begin() == scalingSphereIter) ? nullptr : (*scalingSphereIter)->GetOuterSphere();

	UniquePtr<ScalingSphereBase> scalingSpherePtr = m_attachedSpheres.Remove(scalingSphereIter);

	if (nullptr != pOuterSphereOnHostParticle)
		pOuterSphereOnHostParticle->HandleNewInnerSphere();

	return scalingSpherePtr;
}

// --------------------------------------------------------------------------------------------------------------------------------

Result ParticleBase::ResizeScalingSphere(ScalingSphereBase * pScalingSphereBase, const float trueRadius)
{
	assert(pScalingSphereBase->GetHostParticle() == this);
	assert(GetSphereOfInfluence() != pScalingSphereBase);

	if ((pScalingSphereBase->IsInfluencing() && (GetSphereOfInfluence()->GetTrueRadius() < trueRadius)) ||
		(IsInfluencing() && !pScalingSphereBase->IsInfluencing() && (trueRadius < GetSphereOfInfluence()->GetTrueRadius())))
	{
		return RESULT_CODE_INVALID_PARAMETER; // Changing whether the ScalingSphere is influencing is not currently supported.
	}

	const float previousTrueRadius = pScalingSphereBase->GetTrueRadius();
	ScalingSphereBase *const pPreviousInnerSphere = pScalingSphereBase->GetInnerSphere();

	ScalingSphereList::Iterator scalingSphereIter = m_attachedSpheres.Find(pScalingSphereBase);

	if (m_attachedSpheres.End() == scalingSphereIter)
		return RESULT_CODE_NOT_FOUND;

	pScalingSphereBase->SetTrueRadius(trueRadius);

	const bool hasScalingSphereOrderChanged = m_attachedSpheres.Sort(scalingSphereIter);
	assert(scalingSphereIter->get() == pScalingSphereBase);
	assert(pScalingSphereBase->IsInfluencing() ? ((GetFirstSphere() == pScalingSphereBase) || pScalingSphereBase->GetOuterSphere()->IsInfluencing()) :
		(!IsInfluencing() || (GetSphereOfInfluence() == pScalingSphereBase->GetInnerSphere()) || !pScalingSphereBase->GetInnerSphere()->IsInfluencing()));

	pScalingSphereBase->Initialize();

	if (nullptr != pScalingSphereBase->GetInnerSphere())
		pScalingSphereBase->GetInnerSphere()->Initialize();

	if (hasScalingSphereOrderChanged && (nullptr != pPreviousInnerSphere))
	{
		assert(pPreviousInnerSphere != pScalingSphereBase->GetInnerSphere());
		pPreviousInnerSphere->Initialize();
	}

	pScalingSphereBase->HandleResized(previousTrueRadius);

	if (nullptr != pScalingSphereBase->GetOuterSphere())
	{
		if (m_attachedSpheres.Front().get() == pScalingSphereBase)
			pScalingSphereBase->GetOuterSphere()->HandleParticleSphereResized(pScalingSphereBase);
		else
			pScalingSphereBase->GetOuterSphere()->HandleNewInnerSphere();
	}

	return RESULT_CODE_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------------------------

Result ParticleBase::ResizeSphereOfInfluence(ScalingSphereBase * pSphereOfInfluenceBase, const float trueRadius)
{
	assert(GetSphereOfInfluence() == pSphereOfInfluenceBase);

	assert(false); // TODO...

	return ResizeScalingSphereImpl(pSphereOfInfluenceBase, trueRadius);
}

// --------------------------------------------------------------------------------------------------------------------------------

Result ParticleBase::ResizeScalingSphereImpl(ScalingSphereBase * pScalingSphereBase, const float trueRadius)
{
	assert(false); // TODO...

	return RESULT_CODE_NOT_IMPLEMENTED;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * ParticleBase::GetFirstSphere() const
{
	return m_attachedSpheres.Empty() ? nullptr : m_attachedSpheres.Front().get();
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
