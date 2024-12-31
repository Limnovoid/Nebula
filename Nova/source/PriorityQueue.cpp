#include "PriorityQueue.h"

#include "TestTypes.h"

namespace Nova // ---------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

PriorityQueueTestScript::PriorityQueueTestScript() :
	ITestScript("PriorityQueue")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

PriorityQueueTestScript::~PriorityQueueTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void PriorityQueueTestScript::RunImpl(TestHandler & testHandler)
{
	using Value = LifetimeTracker<size_t>;
	using PriorityQueue = PriorityQueue<Value>;

	static const size_t N_TEST = 10;

	PriorityQueue myPriorityQueue;

	auto fPopRangeTest = [&](size_t index) -> size_t
	{
		myPriorityQueue.Pop();

		testHandler.Assert<size_t, int>([&](int) { return myPriorityQueue.Size(); }, 0, N_TEST - 1 - index, "Size after Pop");
		testHandler.Assert<size_t, int>([&](int) { return Value::GetNumInstances(); }, 0, N_TEST - 1 - index, "Value instances after Pop");

		return myPriorityQueue.Front().m_value;
	};

	testHandler.Assert<size_t, int>([&](int) { return myPriorityQueue.Size(); }, 0, 0, "Empty size");
	testHandler.Assert<bool, int>([&](int) { return myPriorityQueue.Empty(); }, 0, true, "Empty is true");
	testHandler.Assert<size_t, int>([&](int) { return Value::GetNumInstances(); }, 0, 0, "Empty value instances");

	testHandler.Assert<size_t, size_t>([&](size_t index) -> size_t
	{
		myPriorityQueue.Insert(index);

		testHandler.Assert<size_t, int>([&](int) { return myPriorityQueue.Size(); }, 0, index + 1, "Size after Insert");
		testHandler.Assert<size_t, int>([&](int) { return Value::GetNumInstances(); }, 0, index + 1, "Value instances after Insert");

		return myPriorityQueue.Front().m_value;

	}, TestHandler::FRangeIndex(), TestHandler::FRangeZero(), "Value of Front after Inserting incrementing values", TestHandler::IndexRange(0, N_TEST - 1));

	testHandler.Assert<size_t, size_t>(fPopRangeTest, TestHandler::FRangeIndex(), [&](size_t i) { return i + 1; },
		"Value of Front after popping N times", TestHandler::IndexRange(0, N_TEST - 2));

	myPriorityQueue.Pop();

	testHandler.Assert<size_t, int>([&](int) { return myPriorityQueue.Size(); }, 0, 0, "Size after Popping last value");
	testHandler.Assert<bool, int>([&](int) { return myPriorityQueue.Empty(); }, 0, true, "Empty after Popping last value");
	testHandler.Assert<size_t, int>([&](int) { return Value::GetNumInstances(); }, 0, 0, "Value instances after Popping last value");

	testHandler.Assert<size_t, size_t>([&](size_t index) -> size_t
	{
		myPriorityQueue.Insert(index);

		testHandler.Assert<size_t, int>([&](int) { return myPriorityQueue.Size(); }, 0, N_TEST - index, "Size after Insert");
		testHandler.Assert<size_t, int>([&](int) { return Value::GetNumInstances(); }, 0, N_TEST - index, "Value instances after Insert");

		return myPriorityQueue.Front().m_value;

	}, TestHandler::FRangeIndex(), TestHandler::FRangeIndex(), "Value of Front after Inserting decrementing values", TestHandler::IndexRange(N_TEST - 1, 0, -1));

	testHandler.Assert<size_t, size_t>(fPopRangeTest, TestHandler::FRangeIndex(), [&](size_t i) { return i + 1; },
		"Value of Front after popping N times", TestHandler::IndexRange(0, N_TEST - 2));

	myPriorityQueue.Pop();

	testHandler.Assert<size_t, int>([&](int) { return myPriorityQueue.Size(); }, 0, 0, "Size after Popping last value");
	testHandler.Assert<bool, int>([&](int) { return myPriorityQueue.Empty(); }, 0, true, "Empty after Popping last value");
	testHandler.Assert<size_t, int>([&](int) { return Value::GetNumInstances(); }, 0, 0, "Value instances after Popping last value");

	myPriorityQueue.Insert(0);

	testHandler.Assert<size_t, int>([&](int) { return myPriorityQueue.Front().m_value; }, 0, 0, "Front is 0 after inserting 0 into empty queue");

	testHandler.Assert<size_t, size_t>([&](size_t index) -> size_t
	{
		myPriorityQueue.Insert(index);

		return myPriorityQueue.Front().m_value;

	}, TestHandler::FRangeRandomOrder({ 1, N_TEST - 1 }), TestHandler::FRangeZero(), "Value of Front after Inserting values in random order", { 1, N_TEST - 1 });

	testHandler.Assert<size_t, size_t>(fPopRangeTest, TestHandler::FRangeIndex(), [&](size_t i) { return i + 1; },
		"Value of Front after popping N times", TestHandler::IndexRange(0, N_TEST - 2));

	myPriorityQueue.Pop();

	testHandler.Assert<size_t, int>([&](int) { return myPriorityQueue.Size(); }, 0, 0, "Size after Popping last value");
	testHandler.Assert<bool, int>([&](int) { return myPriorityQueue.Empty(); }, 0, true, "Empty after Popping last value");
	testHandler.Assert<size_t, int>([&](int) { return Value::GetNumInstances(); }, 0, 0, "Value instances after Popping last value");
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
