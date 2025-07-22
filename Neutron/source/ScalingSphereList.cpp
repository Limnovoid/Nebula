#include "ScalingSphereList.h"

#include "ParticleBase.h"
#include "ScalingSphereBase.h"
#include "TestHandler.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

bool ScalingSphereListPredicate::operator()(UniquePtr<ScalingSphereBase> const& lhs, UniquePtr<ScalingSphereBase> const& rhs)
{
	return lhs->GetTrueRadius() > rhs->GetTrueRadius();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereList::Iterator ScalingSphereList::Insert(UniquePtr<ScalingSphereBase> &&scalingSphereBasePtr)
{
	Iterator newScalingSphereIter = Base::Emplace(std::forward<UniquePtr<ScalingSphereBase>>(scalingSphereBasePtr));

	InitializeInnerOuter(newScalingSphereIter);

	return newScalingSphereIter;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereList::Iterator ScalingSphereList::Find(ScalingSphereBase * pScalingSphere)
{
	return std::find_if(Begin(), End(), [pScalingSphere](UniquePtr<ScalingSphereBase> const& scalingSphereBasePtr) -> bool
	{
		return (pScalingSphere == scalingSphereBasePtr.get());
	});
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereList::ConstIterator ScalingSphereList::Find(ScalingSphereBase * pScalingSphere) const
{
	return std::find_if(CBegin(), CEnd(), [pScalingSphere](UniquePtr<ScalingSphereBase> const& scalingSphereBasePtr) -> bool
	{
		return (pScalingSphere == scalingSphereBasePtr.get());
	});
}

// --------------------------------------------------------------------------------------------------------------------------------

bool ScalingSphereList::Sort(Iterator &pos)
{
	const bool hasOrderChanged = Base::Sort(pos);

	if (hasOrderChanged)
		InitializeInnerOuter(pos);

	return hasOrderChanged;
}

// --------------------------------------------------------------------------------------------------------------------------------

UniquePtr<ScalingSphereBase> ScalingSphereList::Remove(Iterator pos)
{
	UniquePtr<ScalingSphereBase> returnOwnership(std::move(*pos)); // Transfers ownership to local variable.

	Iterator posToRemove = pos++;

	Erase(posToRemove);

	if ((End() != pos) || (End() != --pos))
		InitializeInnerOuter(pos);

	return returnOwnership;
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereList::InitializeInnerOuter(Iterator pos)
{
	assert(End() != pos);

	ScalingSphereBase *const pScalingSphere = pos->get();

	// Re-initialize previous neighbours to remove the moved Sphere.
	if (nullptr != pScalingSphere->m_pOuterSphere)
		pScalingSphere->m_pOuterSphere->m_pInnerSphere = pScalingSphere->m_pInnerSphere;

	if (nullptr != pScalingSphere->m_pInnerSphere)
		pScalingSphere->m_pInnerSphere->m_pOuterSphere = pScalingSphere->m_pOuterSphere;

	// Update the moved/inserted Sphere and its new neighbours:
	// Outer...
	if (Begin() != pos)
	{
		ScalingSphereList::Iterator outerScalingSphereListIter = pos;
		--outerScalingSphereListIter;

		pScalingSphere->m_pOuterSphere = outerScalingSphereListIter->get();
		(*outerScalingSphereListIter)->m_pInnerSphere = pScalingSphere;
	}
	else
	{
		pScalingSphere->m_pOuterSphere = pScalingSphere->m_pHostParticle->GetHostSphere(); // Note: equals nullptr if host particle is the system host.
	}

	// Inner...
	ScalingSphereList::Iterator innerScalingSphereListIter = pos;
	++innerScalingSphereListIter;

	if (End() == innerScalingSphereListIter)
	{
		pScalingSphere->m_pInnerSphere = nullptr;
	}
	else
	{
		pScalingSphere->m_pInnerSphere = innerScalingSphereListIter->get();
		(*innerScalingSphereListIter)->m_pOuterSphere = pScalingSphere;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereList::Iterator ScalingSphereList::FindSphereOfInfluence()
{
	Iterator pos = Begin();

	while ((End() != pos) && !(*pos)->IsInfluencing())
		++pos;

	return pos;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScalingSphereList::ConstIterator ScalingSphereList::FindSphereOfInfluence() const
{
	ConstIterator pos = CBegin();

	while ((CEnd() != pos) && !(*pos)->IsInfluencing())
		++pos;

	return pos;
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
