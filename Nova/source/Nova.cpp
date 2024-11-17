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

	testHandler.Register(MakeShared<FootprintTestProgram>());
	testHandler.Register(MakeShared<HeapBlockTestProgram>());
	testHandler.Register(MakeShared<HeapBlockListTestProgram>());
	testHandler.Register(MakeShared<AnchoredListTestProgram>());
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
