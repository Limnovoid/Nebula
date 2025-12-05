#include "Nova.h"

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
