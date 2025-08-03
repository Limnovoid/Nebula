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

ScalingSphereBase * ParticleBase::AddScalingSphere(UniquePtr<ScalingSphereBase> &&scalingSphereBasePtr)
{
	ScalingSphereBase * pScalingSphereBase = m_attachedSpheres.Insert(std::forward<UniquePtr<ScalingSphereBase>>(scalingSphereBasePtr))->get();

	pScalingSphereBase->Initialize();

	return pScalingSphereBase;
}

// --------------------------------------------------------------------------------------------------------------------------------

UniquePtr<ScalingSphereBase> ParticleBase::RemoveScalingSphere(ScalingSphereBase * pScalingSphereBase)
{
	UniquePtr<ScalingSphereBase> scalingSpherePtr = nullptr;

	ScalingSphereList::Iterator scalingSphereIter = m_attachedSpheres.Find(pScalingSphereBase);

	if (m_attachedSpheres.End() != scalingSphereIter)
	{
		ScalingSphereBase *const pOuterSphere = (m_attachedSpheres.Begin() == scalingSphereIter) ? nullptr : (*scalingSphereIter)->GetOuterSphere();

		scalingSpherePtr = m_attachedSpheres.Remove(scalingSphereIter);

		if (nullptr != pOuterSphere)
			pOuterSphere->HandleNewInnerSphere();
	}

	return scalingSpherePtr;
}

// --------------------------------------------------------------------------------------------------------------------------------

Result ParticleBase::ResizeScalingSphere(ScalingSphereBase * pScalingSphereBase, const float trueRadius)
{
	assert(this == pScalingSphereBase->GetHostParticle());

	if (GetSphereOfInfluence() == pScalingSphereBase)
		return RESULT_CODE_INVALID_PARAMETER;

	const float previousTrueRadius = pScalingSphereBase->GetTrueRadius();
	ScalingSphereBase *const pPreviousInnerSphere = pScalingSphereBase->GetInnerSphere();

	ScalingSphereList::Iterator scalingSphereIter = m_attachedSpheres.Find(pScalingSphereBase);

	if (m_attachedSpheres.End() == scalingSphereIter)
		return RESULT_CODE_NOT_FOUND;

	pScalingSphereBase->SetTrueRadius(trueRadius);

	const bool hasScalingSphereOrderChanged = m_attachedSpheres.Sort(scalingSphereIter);
	assert(scalingSphereIter->get() == pScalingSphereBase);

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

ScalingSphereBase * ParticleBase::GetFirstSphere() const
{
	return m_attachedSpheres.Empty() ? nullptr : m_attachedSpheres.Front().get();
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
