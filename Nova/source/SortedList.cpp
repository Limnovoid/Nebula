#include "SortedList.h"

#include "TestHandler.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

SortedListTestScript::SortedListTestScript() :
	ITestScript("SortedList")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

SortedListTestScript::~SortedListTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void SortedListTestScript::RunImpl(TestHandler & testHandler)
{
	SortedList<int64_t> sortedList;

	testHandler.Assert<int64_t, int64_t>([&](int64_t index) -> int64_t
	{
		TestHandler::OutputMode const outputMode = testHandler.SetOutputMode(TestHandler::OutputMode::SILENT);

		SortedList<int64_t>::iterator iter = sortedList.Insert(index);

		for (SortedList<int64_t>::const_iterator iterNext = sortedList.cbegin(), iter = iterNext++;
			sortedList.cend() != iterNext;
			++iter, ++iterNext)
		{
			testHandler.Assert(*iter < *iterNext, true, "Order preserved");
		}

		testHandler.SetOutputMode(outputMode);

		return sortedList.size();

	}, TestHandler::FRangeRandomOrder({ 1, 10 }), TestHandler::FRangeIndex(), "Insert", { 1, 10 });
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
