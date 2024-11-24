#include "HeapBlock.h"

#include "TestHandler.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

HeapBlock::HeapBlock() :
	m_footprint(),
	m_pBlock(nullptr)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

HeapBlock::~HeapBlock()
{
	if (IsInitialized())
		FreeBlock(m_pBlock);
}

// --------------------------------------------------------------------------------------------------------------------------------

void HeapBlock::Release()
{
	ASSERT_THROW(IsInitialized(), RESULT_CODE_NOT_INITIALIZED, "");

	FreeBlock(m_pBlock);

	m_pBlock = nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockTestProgram::HeapBlockTestProgram() :
	ITestProgram("HeapBlock")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockTestProgram::~HeapBlockTestProgram()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void HeapBlockTestProgram::RunImpl(TestHandler & testHandler)
{
	HeapBlock heapBlock;

	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlock.IsInitialized(); }, 0, false, "Default constructed HeapBlock::IsInitialized()");

	testHandler.Assert<ResultCode, size_t>([&](size_t index) -> ResultCode
	{
		try { heapBlock.Get(index); }
		catch (AssertionException const& exception)
		{
			return exception.GetResult().GetCode();
		}
		return RESULT_CODE_SUCCESS;
	}, 0, RESULT_CODE_NOT_INITIALIZED, "Default constructed HeapBlock::Size()");

	static size_t const TEST_SIZE = 64;
	heapBlock.Initialize(Footprint::Make<byte_t>(TEST_SIZE));
	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlock.IsInitialized(); }, 0, true);
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlock.Size(); }, 0, TEST_SIZE);

	heapBlock.Resize(Footprint::Make<byte_t>(TEST_SIZE / 2));
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlock.Size(); }, 0, TEST_SIZE / 2);

	HeapBlock heapBlock2(Footprint::Make<byte_t>(TEST_SIZE));
	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlock2.IsInitialized(); }, 0, true);
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlock2.Size(); }, 0, TEST_SIZE * sizeof(uint8_t));

	HeapBlock heapBlock3(Footprint::Make<uint64_t>(TEST_SIZE));
	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlock3.IsInitialized(); }, 0, true);
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlock3.Size(); }, 0, TEST_SIZE * sizeof(uint64_t));

	uint64_t * pBlock = heapBlock3.Get<uint64_t>();
	testHandler.Assert<uintptr_t, size_t>([&](size_t index) -> uintptr_t
	{
		return reinterpret_cast<uintptr_t>(heapBlock3.Get<uint64_t>(index));
	}, TEST_SIZE / 2, reinterpret_cast<uintptr_t>(pBlock + (TEST_SIZE / 2)));

	Footprint footprint1 = Footprint::Make<uint16_t>(TEST_SIZE);
	HeapBlock heapBlock4(footprint1);
	testHandler.Assert<size_t, size_t, TestHandler::FRangeIndex, TestHandler::FRangeZero>([&](size_t index) -> size_t
	{
		return reinterpret_cast<uintptr_t>(heapBlock4.Get<uint16_t>(index)) % footprint1.Alignment();
	}, TestHandler::FRangeIndex(), TestHandler::FRangeZero(), "HeapBlock.Get(i) alignment", { 0, TEST_SIZE - 1 });
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
