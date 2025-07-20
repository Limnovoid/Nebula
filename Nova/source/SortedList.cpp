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

		SortedList<int64_t>::Iterator iter = sortedList.Insert(index);

		for (SortedList<int64_t>::ConstIterator iterNext = sortedList.CBegin(), iter = iterNext++;
			sortedList.CEnd() != iterNext;
			++iter, ++iterNext)
		{
			testHandler.Assert(*iter < *iterNext, true, "Order preserved");
		}

		testHandler.SetOutputMode(outputMode);

		return sortedList.Size();

	}, TestHandler::FRangeRandomOrder({ 1, I_MAX }), TestHandler::FRangeIndex(), "Insert", { 1, I_MAX });

	testHandler.Assert<int64_t, int64_t>([&](int64_t index)->int64_t
	{
		TestHandler::OutputMode const outputMode = testHandler.SetOutputMode(TestHandler::OutputMode::SILENT);

		SortedList<int64_t>::Iterator pos = sortedList.Find(index);
		*pos += 100;
		sortedList.Sort(pos);

		for (SortedList<int64_t>::ConstIterator iterNext = sortedList.CBegin(), iter = iterNext++;
			sortedList.CEnd() != iterNext;
			++iter, ++iterNext)
		{
			testHandler.Assert(*iter < *iterNext, true, "Order preserved");
		}

		testHandler.SetOutputMode(outputMode);

		return sortedList.Size();

	}, TestHandler::FRangeRandomOrder({ 1, I_MAX }), TestHandler::FRangeConstant(I_MAX), "Modify and Sort", { 1, I_MAX });

	testHandler.Assert<int64_t, int64_t>([&](int64_t index) -> int64_t
	{
		TestHandler::OutputMode const outputMode = testHandler.SetOutputMode(TestHandler::OutputMode::SILENT);

		SortedList<int64_t>::Iterator iterToRemove = sortedList.Begin();

		for (size_t i = 1; i < Random::Integer(1ull, sortedList.Size()); ++i)
			++iterToRemove;

		sortedList.Erase(iterToRemove);

		for (SortedList<int64_t>::ConstIterator iterNext = sortedList.CBegin(), iter = iterNext++;
			sortedList.CEnd() != iterNext;
			++iter, ++iterNext)
		{
			testHandler.Assert(*iter < *iterNext, true, "Order preserved");
		}

		testHandler.SetOutputMode(outputMode);

		return sortedList.Size();

	}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Erase", { I_MAX - 1, 1, -1 });
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
