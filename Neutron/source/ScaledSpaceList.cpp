#include "ScaledSpaceList.h"

#include "ParticleBase.h"
#include "ScaledSpaceBase.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

bool ScaledSpaceListPredicate::operator()(UniquePtr<ScaledSpaceBase> const& lhs, UniquePtr<ScaledSpaceBase> const& rhs)
{
	return lhs->GetTrueRadius() > rhs->GetTrueRadius();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

void ScaledSpaceList::InitializeInnerOuter(iterator newScaledSpaceIter)
{
	if (begin() != newScaledSpaceIter)
	{
		ScaledSpaceList::iterator outerScalingSpaceListIter = newScaledSpaceIter;
		--outerScalingSpaceListIter;

		(*newScaledSpaceIter)->m_pOuterSpace = outerScalingSpaceListIter->get();
		(*outerScalingSpaceListIter)->m_pInnerSpace = newScaledSpaceIter->get();
	}
	else
	{
		(*newScaledSpaceIter)->m_pOuterSpace = (*newScaledSpaceIter)->m_pHostParticle->GetHostSpace(); // Note: equals nullptr if host particle is the system host.
	}

	ScaledSpaceList::iterator innerScalingSpaceListIter = newScaledSpaceIter;
	++innerScalingSpaceListIter;
	if (end() != innerScalingSpaceListIter)
	{
		(*newScaledSpaceIter)->m_pInnerSpace = innerScalingSpaceListIter->get();
		(*innerScalingSpaceListIter)->m_pOuterSpace = newScaledSpaceIter->get();
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

ScaledSpaceList::iterator ScaledSpaceList::FindSpaceOfInfluence()
{
	iterator spaceOfInfluenceIter = begin();

	while ((end() != spaceOfInfluenceIter) && !(*spaceOfInfluenceIter)->IsInfluencing())
		++spaceOfInfluenceIter;

	return spaceOfInfluenceIter;
}

// --------------------------------------------------------------------------------------------------------------------------------

ScaledSpaceList::const_iterator ScaledSpaceList::FindSpaceOfInfluence() const
{
	const_iterator spaceOfInfluenceIter = cbegin();

	while ((cend() != spaceOfInfluenceIter) && !(*spaceOfInfluenceIter)->IsInfluencing())
		++spaceOfInfluenceIter;

	return spaceOfInfluenceIter;
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
