#include "Nova.h"

// Include all project headers for library building.
#include "BinaryTree.h"
#include "RedBlackTree.h"
#include "HeapBlock.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

void AddTests(Nebula::TestHandler & testHandler)
{
	using namespace Nebula;

	testHandler.Register(MakeShared<HeapBlockTestProgram>());
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
