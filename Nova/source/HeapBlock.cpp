#include "HeapBlock.h"

#include "TestHandler.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

HeapBlock::HeapBlock() :
	m_size(0),
	m_pBlock(nullptr)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
HeapBlock::HeapBlock(Settings<TValue> settings) :
	m_size(sizeof(TValue) * settings.m_count),
	m_pBlock(AllocateBlock<TValue>(settings.m_count))
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
	EXCEPTION_ASSERT(IsInitialized(), RESULT_CODE_NOT_INITIALIZED, "");

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
	}, 0, RESULT_CODE_NOT_INITIALIZED, "Default constructed HeapBlock::Get()");

	static size_t const TEST_SIZE = 64;
	heapBlock.Initialize(TEST_SIZE);
	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlock.IsInitialized(); }, 0, true);
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlock.Size(); }, 0, TEST_SIZE);

	heapBlock.Resize(TEST_SIZE / 2);
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlock.Size(); }, 0, TEST_SIZE / 2);

	HeapBlock heapBlock2({ TEST_SIZE });
	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlock2.IsInitialized(); }, 0, true);
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlock2.Size(); }, 0, TEST_SIZE * sizeof(uint8_t));

	HeapBlock heapBlock3(HeapBlock::Settings<uint64_t>{ TEST_SIZE });
	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlock3.IsInitialized(); }, 0, true);
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlock3.Size(); }, 0, TEST_SIZE * sizeof(uint64_t));

	uint64_t * pBlock = heapBlock3.Get<uint64_t>();
	testHandler.Assert<uintptr_t, size_t>([&](size_t index) -> uintptr_t
	{
		return reinterpret_cast<uintptr_t>(heapBlock3.Get<uint64_t>(index));
	}, TEST_SIZE / 2, reinterpret_cast<uintptr_t>(pBlock + (TEST_SIZE / 2)));
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------