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
	static constexpr int64_t I_MAX = 100;

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

	}, TestHandler::FRangeRandomOrder({ 1, I_MAX }), TestHandler::FRangeIndex(), "Insert", { 1, I_MAX });

	testHandler.Assert<int64_t, int64_t>([&](int64_t index)->int64_t
	{
		TestHandler::OutputMode const outputMode = testHandler.SetOutputMode(TestHandler::OutputMode::SILENT);

		SortedList<int64_t>::iterator pos = sortedList.Find(index);
		*pos += 100;
		sortedList.Sort(pos);

		for (SortedList<int64_t>::const_iterator iterNext = sortedList.cbegin(), iter = iterNext++;
			sortedList.cend() != iterNext;
			++iter, ++iterNext)
		{
			testHandler.Assert(*iter < *iterNext, true, "Order preserved");
		}

		testHandler.SetOutputMode(outputMode);

		return sortedList.size();

	}, TestHandler::FRangeRandomOrder({ 1, I_MAX }), TestHandler::FRangeConstant(I_MAX), "Modify and Sort", { 1, I_MAX });

	testHandler.Assert<int64_t, int64_t>([&](int64_t index) -> int64_t
	{
		TestHandler::OutputMode const outputMode = testHandler.SetOutputMode(TestHandler::OutputMode::SILENT);

		SortedList<int64_t>::iterator iterToRemove = sortedList.begin();

		for (size_t i = 0; i < Random::Integer(0ull, sortedList.size()); ++i)
			++iterToRemove;

		sortedList.Erase(iterToRemove);

		for (SortedList<int64_t>::const_iterator iterNext = sortedList.cbegin(), iter = iterNext++;
			sortedList.cend() != iterNext;
			++iter, ++iterNext)
		{
			testHandler.Assert(*iter < *iterNext, true, "Order preserved");
		}

		testHandler.SetOutputMode(outputMode);

		return sortedList.size();

	}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Erase", { I_MAX - 1, 1, -1 });
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
