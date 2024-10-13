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
HeapBlock::HeapBlock(size_t count) :
	m_size(sizeof(TValue) * count),
	m_pBlock(AllocateBlock<TValue>(count))
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

	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlock.IsInitialized(); }, 0, false);
	
	static size_t const TEST_SIZE = 64;
	heapBlock.Initialize(TEST_SIZE);

	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlock.IsInitialized(); }, 0, true);
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlock.Size(); }, 0, TEST_SIZE);
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
