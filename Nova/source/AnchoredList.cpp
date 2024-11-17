#include "AnchoredList.h"

#include "Random.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

AnchoredListTestProgram::AnchoredListTestProgram() :
	ITestProgram("AnchoredList")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

AnchoredListTestProgram::~AnchoredListTestProgram()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void AnchoredListTestProgram::RunImpl(TestHandler & testHandler)
{
	using List = AnchoredList<String>;
	AnchoredList<String> anchoredList;

	testHandler.Assert<size_t, int>([&](int) -> size_t { return anchoredList.m_length; }, 0, 1, "Default length");

	testHandler.Assert<List::Iterator, int>([&](int) -> List::Iterator { return anchoredList.begin(); }, 0, anchoredList.end(),
		"Default begin() = end()");

	testHandler.Assert<List::ConstIterator, int>([&](int) -> List::ConstIterator { return anchoredList.cbegin(); }, 0, anchoredList.cend(),
		"Default cbegin() = cend()");

	testHandler.Assert<uintptr_t, int>([&](int) -> size_t { return reinterpret_cast<uintptr_t>(anchoredList.m_pTail); },
		0, reinterpret_cast<uintptr_t>(&anchoredList.m_head), "Default tail points to head");

	for (size_t iTest = 0; iTest < 10; ++iTest)
	{
		AnchoredList<size_t> integers;

		size_t const nAdd = Random::Integer(1, 10);

		for (size_t iAdd = 0; iAdd < nAdd; ++iAdd)
			integers.EmplaceBack(1 + iAdd);

		testHandler.Assert
	}
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
