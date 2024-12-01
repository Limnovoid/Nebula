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

	testHandler.Assert<size_t, int>([&](int) -> size_t { return anchoredList.Size(); }, 0, 1, "Default size");

	testHandler.Assert<List::Iterator, int>([&](int) -> List::Iterator { return anchoredList.begin(); }, 0, anchoredList.end(),
		"Default begin() = end()");

	testHandler.Assert<List::ConstIterator, int>([&](int) -> List::ConstIterator { return anchoredList.cbegin(); }, 0, anchoredList.cend(),
		"Default cbegin() = cend()");

	testHandler.Assert<uintptr_t, int>([&](int) -> size_t { return reinterpret_cast<uintptr_t>(anchoredList.m_pTail); },
		0, reinterpret_cast<uintptr_t>(&anchoredList.m_head), "Default tail points to head");

	{
		static const size_t N_TEST = 10;

		using MyAnchoredList = AnchoredList<LifetimeTracker<size_t>>;

		MyAnchoredList integers;

		testHandler.Assert<size_t, int>([&](int) -> size_t
		{
			return LifetimeTracker<size_t>::GetNumInstances();

		}, 0, 1, "Num. value constructions");

		testHandler.Assert<size_t, size_t>([&](size_t iTest) -> size_t
		{
			for (size_t iAdd = 0; iAdd < iTest; ++iAdd)
				integers.EmplaceBack(1 + iAdd);

			integers.Reset();

			return integers.Size();

		}, TestHandler::FRangeIndex(), TestHandler::FRangeConstant(1), "Size after Reset()", TestHandler::IndexRange(1, N_TEST));

		testHandler.Assert<size_t, size_t>([&](size_t iTest) -> size_t
		{
			integers.Reset();

			for (size_t iAdd = 0; iAdd < iTest; ++iAdd)
				integers.EmplaceBack(1 + iAdd);

			return integers.Size() - 1; // Number of nodes added = final total minus the head.

		}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Size after EmplaceBack(n)", TestHandler::IndexRange(1, N_TEST));

		testHandler.Assert<size_t, size_t>([&](size_t iTest) -> size_t
		{
			return (*integers.At(iTest)).m_value;

		}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Value stored at At(n)", TestHandler::IndexRange(0, N_TEST));

		testHandler.Assert<MyAnchoredList::Iterator, size_t>([&](size_t iLast) -> MyAnchoredList::Iterator
		{
			return integers.At(iLast);

		}, N_TEST, integers.end(), "Iterator returned by At(nMax) is end()");

		testHandler.Assert<MyAnchoredList::Iterator, size_t>([&](size_t iTest) -> MyAnchoredList::Iterator
		{
			MyAnchoredList::Iterator iterator = integers.begin();

			for (size_t i = 0; i < iTest; ++i)
				++iterator;

			return iterator;

		}, TestHandler::FRangeIndex(), [&](size_t i) { return integers.At(i); }, "Iterator pre-increment", TestHandler::IndexRange(0, N_TEST));

		//testHandler.Assert<MyAnchoredList::Iterator, size_t>([&](size_t iTest) -> MyAnchoredList::Iterator
		//{
		//	integers.Resize()
		//
		//}, TestHandler::FRangeIndex(), [&](size_t i) { return integers.At(i); }, "Iterator pre-increment", TestHandler::IndexRange(0, N_TEST));
	}
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
