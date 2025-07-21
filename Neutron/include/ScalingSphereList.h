#ifndef NEUTRON_SCALED_SPACE_LIST_H
#define NEUTRON_SCALED_SPACE_LIST_H

#include "NebulaTypes.h"
#include "SortedList.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;
using namespace Nova;

class ScalingSphereBase;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

struct ScalingSphereListPredicate
{
	bool operator()(UniquePtr<ScalingSphereBase> const& lhs, UniquePtr<ScalingSphereBase> const& rhs);
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ScalingSphereList : protected SortedList<UniquePtr<ScalingSphereBase>, ScalingSphereListPredicate>
{
	using Base = SortedList<UniquePtr<ScalingSphereBase>, ScalingSphereListPredicate>;

public:
	using Base::Iterator;
	using Base::ConstIterator;
	using Base::Empty;
	using Base::Size;
	using Base::Begin;
	using Base::CBegin;
	using Base::End;
	using Base::CEnd;
	using Base::Front;
	using Base::Back;
	using Base::Sort;

	Iterator Insert(UniquePtr<ScalingSphereBase> && scalingSphereBasePtr);

	Iterator Find(ScalingSphereBase * pScalingSphere);
	ConstIterator Find(ScalingSphereBase * pScalingSphere) const;

	UniquePtr<ScalingSphereBase> Remove(Iterator pos);

	Iterator FindSphereOfInfluence();
	ConstIterator FindSphereOfInfluence() const;

private:
	void InitializeInnerOuter(Iterator pos);
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_SCALED_SPACE_LIST_H
