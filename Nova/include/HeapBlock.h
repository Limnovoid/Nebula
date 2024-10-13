#ifndef NOVA_HEAP_BLOCK_H
#define NOVA_HEAP_BLOCK_H

#include "NebulaTypes.h"
#include "Result.h"
#include "Macros.h"
#include "TestHandler.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

class HeapBlock
{
public:
	HeapBlock();

	template<typename TValue = byte_t>
	HeapBlock(size_t count);

	~HeapBlock();

	/// <summary>
	/// Initialize the object with a contiguous block of memory from the heap.
	/// The allocated block is aligned to type TValue.
	/// </summary>
	/// <typeparam name="TValue">The type to use as a basis for allocating the block from the heap.</typeparam>
	/// <param name="count"> The number of TValue objects to allocate for - the allocated block will have size = count * sizeof(TValue) </param>
	/// <exception cref="AssertionException"> Already initialized - a block is already allocated. </exception>
	template<typename TValue = byte_t>
	void Initialize(size_t count);

	/// <summary> Free the held block of memory. </summary>
	/// <exception cref="AssertionException"> Not initialized - no block to free. </exception>
	void Release();

	/// <summary> Resize the block. Frees the existing block (if initialized) and acquires a new block from the heap. </summary>
	template<typename TValue = byte_t>
	void Resize(size_t count);

	/// <returns> Whether the object has a reference to an existing block of memory. </returns>
	bool IsInitialized() const;

	/// <returns> The size of the allocated block as a number of TValues = total bytes / sizeof(TValue). </summary>
	template<typename TValue = byte_t>
	size_t Size() const;

private:
	template<typename TValue>
	static byte_t * AllocateBlock(size_t count);

	static void FreeBlock(byte_t * pBlock);

	size_t		m_size;
	byte_t *	m_pBlock;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
inline void HeapBlock::Initialize(size_t count)
{
	EXCEPTION_ASSERT(!IsInitialized(), RESULT_CODE_ALREADY_INITIALIZED, "");

	m_pBlock = AllocateBlock<TValue>(count);
	m_size = sizeof(TValue) * count;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
inline void HeapBlock::Resize(size_t count)
{
	EXCEPTION_ASSERT(IsInitialized(), RESULT_CODE_NOT_INITIALIZED, "");

	FreeBlock(m_pBlock);

	m_pBlock = AllocateBlock<TValue>(count);

	m_size = sizeof(TValue) * count;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool HeapBlock::IsInitialized() const
{
	ASSERT((nullptr != m_pBlock) == (0 < m_size));

	return (nullptr != m_pBlock);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
inline size_t HeapBlock::Size() const
{
	if (0 == m_size)
		return 0;

	return m_size / sizeof(TValue);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
inline byte_t * HeapBlock::AllocateBlock(size_t count)
{
	return reinterpret_cast<byte_t *>(_aligned_malloc(count * sizeof(TValue), alignof(TValue)));
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void HeapBlock::FreeBlock(byte_t * pBlock)
{
	_aligned_free(pBlock);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class HeapBlockTestProgram : public ITestProgram
{
public:
	HeapBlockTestProgram();
	virtual ~HeapBlockTestProgram();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Nova ---------------------------------------------------------------------------------------------------------------

#endif//NOVA_HEAP_BLOCK_H
