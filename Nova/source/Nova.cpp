#include "Nova.h"

// Include all project headers for library building.
#include "AnchoredList.h"
#include "BinaryTree.h"
#include "Footprint.h"
#include "HeapBlock.h"
#include "HeapBlockList.h"
#include "PriorityQueue.h"
#include "RedBlackTree.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

void AddTests(Nebula::TestHandler & testHandler)
{
	using namespace Nebula;

	testHandler.Register(MakeShared<FootprintTestScript>());
	testHandler.Register(MakeShared<HeapBlockTestScript>());
	testHandler.Register(MakeShared<HeapBlockListTestScript>());
	testHandler.Register(MakeShared<AnchoredListTestScript>());
	testHandler.Register(MakeShared<PriorityQueueTestScript>());
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
