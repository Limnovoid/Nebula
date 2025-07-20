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

UniquePtr<ScalingSphereBase> ScalingSphereList::Remove(Iterator pos)
{
	UniquePtr<ScalingSphereBase> returnOwnership(std::move(*pos)); // Transfers ownership to local variable.

	Iterator posToRemove = pos++;

	Erase(posToRemove);

	if (End() != pos)
		InitializeInnerOuter(pos);

	return returnOwnership;
}

// --------------------------------------------------------------------------------------------------------------------------------

void ScalingSphereList::InitializeInnerOuter(Iterator pos)
{
	assert(End() != pos);

	if (Begin() != pos)
	{
		ScalingSphereList::Iterator outerScalingSpaceListIter = pos;
		--outerScalingSpaceListIter;

		(*pos)->m_pOuterSpace = outerScalingSpaceListIter->get();
		(*outerScalingSpaceListIter)->m_pInnerSpace = pos->get();
	}
	else
	{
		(*pos)->m_pOuterSpace = (*pos)->m_pHostParticle->GetHostSphere(); // Note: equals nullptr if host particle is the system host.
	}

	ScalingSphereList::Iterator innerScalingSpaceListIter = pos;
	++innerScalingSpaceListIter;

	if (End() == innerScalingSpaceListIter)
	{
		(*pos)->m_pInnerSpace = nullptr;
	}
	else
	{
		(*pos)->m_pInnerSpace = innerScalingSpaceListIter->get();
		(*innerScalingSpaceListIter)->m_pOuterSpace = pos->get();
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
