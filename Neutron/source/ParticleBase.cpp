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
	ScalingSphereList::Iterator scalingSphereIter = m_attachedSpheres.Find(pScalingSphereBase);

	if (m_attachedSpheres.End() != scalingSphereIter)
		return m_attachedSpheres.Remove(scalingSphereIter);

	return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------

Result ParticleBase::ResizeScalingSphere(ScalingSphereBase * pScalingSphereBase, const float trueRadius)
{
	assert(this == pScalingSphereBase->GetHostParticle());

	const float previousTrueRadius = pScalingSphereBase->GetTrueRadius();
	ScalingSphereBase *const pPreviousInnerSphere = pScalingSphereBase->GetInnerSpace();

	ScalingSphereList::Iterator scalingSphereIter = m_attachedSpheres.Find(pScalingSphereBase);

	if (m_attachedSpheres.End() == scalingSphereIter)
		return RESULT_CODE_NOT_FOUND;

	pScalingSphereBase->SetTrueRadius(trueRadius);

	scalingSphereIter = m_attachedSpheres.Sort(scalingSphereIter);
	assert(scalingSphereIter->get() == pScalingSphereBase);

	pScalingSphereBase->Initialize();

	if (nullptr != pScalingSphereBase->GetInnerSpace())
		pScalingSphereBase->GetInnerSpace()->Initialize();

	if ((nullptr != pPreviousInnerSphere) && (pPreviousInnerSphere != pScalingSphereBase->GetInnerSpace()))
		pPreviousInnerSphere->Initialize();

	pScalingSphereBase->HandleResized(previousTrueRadius);

	return RESULT_CODE_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereBase * ParticleBase::GetFirstSphere() const
{
	return m_attachedSpheres.Empty() ? nullptr : m_attachedSpheres.Front().get();
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
