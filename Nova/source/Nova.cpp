#include "Nova.h"

// Include all project headers for library building.
#include "AnchoredList.h"
#include "BinaryTree.h"
#include "Footprint.h"
#include "HeapBlock.h"
#include "HeapBlockList.h"
#include "PriorityQueue.h"
#include "RedBlackTree.h"
#include "SortedList.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

void AddTests(Nebula::TestHandler & testHandler)
{
	using namespace Nebula;

	testHandler.Register(MakeShared<FootprintTestScript>(), "Nova");
	testHandler.Register(MakeShared<HeapBlockTestScript>(), "Nova");
	testHandler.Register(MakeShared<HeapBlockListTestScript>(), "Nova");
	testHandler.Register(MakeShared<AnchoredListTestScript>(), "Nova");
	testHandler.Register(MakeShared<PriorityQueueTestScript>(), "Nova");
	testHandler.Register(MakeShared<SortedListTestScript>(), "Nova");
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
