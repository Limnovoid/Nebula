#include "Nova.h"

// Include all project headers for library building.
#include "BinaryTree.h"
#include "Footprint.h"
#include "RedBlackTree.h"
#include "HeapBlock.h"
#include "HeapBlockList.h"
#include "AnchoredList.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

void AddTests(Nebula::TestHandler & testHandler)
{
	using namespace Nebula;

	testHandler.Register(MakeShared<FootprintTestScript>());
	testHandler.Register(MakeShared<HeapBlockTestScript>());
	testHandler.Register(MakeShared<HeapBlockListTestScript>());
	testHandler.Register(MakeShared<AnchoredListTestScript>());
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
