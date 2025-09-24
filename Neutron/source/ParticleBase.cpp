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

	if ((nullptr != pOuterSphereOnHostParticle) && (nullptr != pOuterSphereOnHostParticle->GetInnerSphere()))
		pOuterSphereOnHostParticle->HandleNewInnerSphere();

	return scalingSpherePtr;
}

// --------------------------------------------------------------------------------------------------------------------------------

Result ParticleBase::ResizeScalingSphere(ScalingSphereBase * pScalingSphereBase, Unit::Absolute const& absoluteRadius)
{
	assert(pScalingSphereBase->GetHostParticle() == this);
	assert(GetSphereOfInfluence() != pScalingSphereBase);

	if ((pScalingSphereBase->IsInfluencing() && (GetSphereOfInfluence()->GetAbsoluteRadius() < absoluteRadius)) ||
		(IsInfluencing() && !pScalingSphereBase->IsInfluencing() && (absoluteRadius < GetSphereOfInfluence()->GetAbsoluteRadius())))
	{
		return RESULT_CODE_INVALID_PARAMETER; // Changing whether the ScalingSphere is influencing is not currently supported.
	}

	const Unit::Absolute previousAbsoluteRadius = pScalingSphereBase->GetAbsoluteRadius();
	ScalingSphereBase *const pPreviousInnerSphere = pScalingSphereBase->GetInnerSphere();

	ScalingSphereList::Iterator scalingSphereIter = m_attachedSpheres.Find(pScalingSphereBase);

	if (m_attachedSpheres.End() == scalingSphereIter)
		return RESULT_CODE_NOT_FOUND;

	pScalingSphereBase->SetAbsoluteRadius(absoluteRadius);

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

	pScalingSphereBase->HandleResized(previousAbsoluteRadius);

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

Result ParticleBase::ResizeSphereOfInfluence(Unit::Absolute const& absoluteRadius)
{
	assert(IsInfluencing());

	ScalingSphereBase * pScalingSphereBase = GetSphereOfInfluence();

	assert(false); // TODO...

	return ResizeScalingSphereImpl(pScalingSphereBase, absoluteRadius);
}

// --------------------------------------------------------------------------------------------------------------------------------

Result ParticleBase::ResizeScalingSphereImpl(ScalingSphereBase * pScalingSphereBase, Unit::Absolute const& absoluteRadius)
{
	assert(false); // TODO - refactor common code from ResizeScalingSphere() into here...

	return RESULT_CODE_NOT_IMPLEMENTED;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * ParticleBase::GetFirstSphere() const
{
	return m_attachedSpheres.Empty() ? nullptr : m_attachedSpheres.Front().get();
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
