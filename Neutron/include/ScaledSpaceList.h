#ifndef NEUTRON_SCALED_SPACE_LIST_H
#define NEUTRON_SCALED_SPACE_LIST_H

#include "NebulaTypes.h"
#include "SortedList.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;
using namespace Nova;

class ParticleBase;
class ScaledSpaceBase;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

struct ScaledSpaceListPredicate
{
	bool operator()(UniquePtr<ScaledSpaceBase> const& lhs, UniquePtr<ScaledSpaceBase> const& rhs);
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ScaledSpaceList : public SortedList<UniquePtr<ScaledSpaceBase>, ScaledSpaceListPredicate>
{
	using Base = SortedList<UniquePtr<ScaledSpaceBase>, ScaledSpaceListPredicate>;

public:
	using iterator = Base::iterator;
	using const_iterator = Base::const_iterator;
	using Base::SortedList;
	using Base::begin;
	using Base::end;
	using Base::cbegin;
	using Base::cend;
	using Base::size;
	using Base::empty;
	using Base::clear;

	ScaledSpaceBase const& Front() const;
	ScaledSpaceBase const& Back() const;

	template<typename TScaledSpace>
	iterator Emplace(ParticleBase * pHostParticle, float trueRadius);

	iterator FindSpaceOfInfluence();
	const_iterator FindSpaceOfInfluence() const;

private:
	void InitializeInnerOuter(iterator newScaledSpaceIter);
};

// --------------------------------------------------------------------------------------------------------------------------------

inline ScaledSpaceBase const& ScaledSpaceList::Front() const
{
	assert(!empty());

	return **cbegin();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ScaledSpaceBase const& ScaledSpaceList::Back() const
{
	assert(!empty());

	return **(--cend());
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TScaledSpace>
inline ScaledSpaceList::iterator ScaledSpaceList::Emplace(ParticleBase * pHostParticle, float trueRadius)
{
	iterator newScaledSpaceIter = Base::Emplace(std::move(MakeUnique<TScaledSpace>(pHostParticle, trueRadius)));

	InitializeInnerOuter(newScaledSpaceIter);

	return newScaledSpaceIter;
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_SCALED_SPACE_LIST_H
