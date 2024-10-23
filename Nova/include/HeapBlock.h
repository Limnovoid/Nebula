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
	template<typename TValue>
	struct Settings
	{
		using Type = TValue;

		size_t	m_count;
	};

	HeapBlock();

	template<typename TValue = byte_t>
	HeapBlock(Settings<TValue> settings);

	~HeapBlock();

	/// <returns> Whether the object has a reference to an existing block of memory. </returns>
	bool IsInitialized() const;

	/// <returns> The size of the allocated block as a number of TValues = total bytes / sizeof(TValue), or 0 if not initialized. </summary>
	template<typename TValue = byte_t>
	size_t Size() const;

	/// <summary>
	/// Get the address at a given index. Indices are spaced according to the size of TValue.
	/// Note: does *not* prevent indexing beyond the end of the block.
	/// </summary>
	/// <exception cref="AssertionException">
	/// Not Initialized - no memory block to index.
	/// Invalid Parameter - alignment mistmatch between TValue and the allocated block.
	/// </exception>
	template<typename TValue = byte_t>
	TValue const* Get(size_t index = 0) const;

	/// <summary>
	/// Get the address at a given index. Indices are spaced according to the size of TValue.
	/// Note: does *not* prevent indexing beyond the end of the block.
	/// </summary>
	/// <exception cref="AssertionException">
	/// Not Initialized - no memory block to index.
	/// Invalid Parameter - alignment mistmatch between TValue and the allocated block.
	/// </exception>
	template<typename TValue = byte_t>
	TValue * Get(size_t index = 0);

	/// <summary>
	/// Initialize the object with a contiguous block of memory from the heap.
	/// The allocated block is aligned to type TValue.
	/// </summary>
	/// <typeparam name="TValue">The type to use as a basis for allocating the block from the heap.</typeparam>
	/// <param name="count"> The number of TValue objects to allocate for - the allocated block will have size = count * sizeof(TValue) </param>
	/// <exception cref="AssertionException"> Already Initialized - a block is already allocated. </exception>
	template<typename TValue = byte_t>
	TValue * Initialize(size_t count);

	/// <summary> Free the held block of memory. </summary>
	/// <exception cref="AssertionException"> Not Initialized - no block to free. </exception>
	void Release();

	/// <summary> Resize the block. Frees the existing block (if initialized) and acquires a new block from the heap. </summary>
	template<typename TValue = byte_t>
	TValue * Resize(size_t count);

	byte_t const* operator[](size_t index) const;
	byte_t * operator[](size_t index);

private:
	template<typename TValue>
	static byte_t * AllocateBlock(size_t count);

	static void FreeBlock(byte_t * pBlock);

	size_t		m_size;
	byte_t *	m_pBlock;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
inline TValue const* HeapBlock::Get(size_t index) const
{
	EXCEPTION_ASSERT(IsInitialized(), RESULT_CODE_NOT_INITIALIZED, "");
	EXCEPTION_ASSERT(0 == (reinterpret_cast<uintptr_t>(m_pBlock) % alignof(TValue)), RESULT_CODE_INVALID_PARAMETER, "");

	return reinterpret_cast<TValue const*>(m_pBlock) + index;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
inline TValue * HeapBlock::Get(size_t index)
{
	EXCEPTION_ASSERT(IsInitialized(), RESULT_CODE_NOT_INITIALIZED, "");
	EXCEPTION_ASSERT(0 == (reinterpret_cast<uintptr_t>(m_pBlock) % alignof(TValue)), RESULT_CODE_INVALID_PARAMETER, "");

	return reinterpret_cast<TValue *>(m_pBlock) + index;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
inline TValue * HeapBlock::Initialize(size_t count)
{
	EXCEPTION_ASSERT(!IsInitialized(), RESULT_CODE_ALREADY_INITIALIZED, "");

	m_pBlock = AllocateBlock<TValue>(count);
	m_size = sizeof(TValue) * count;

	return reinterpret_cast<TValue *>(m_pBlock);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
inline TValue * HeapBlock::Resize(size_t count)
{
	EXCEPTION_ASSERT(IsInitialized(), RESULT_CODE_NOT_INITIALIZED, "");

	FreeBlock(m_pBlock);

	m_pBlock = AllocateBlock<TValue>(count);

	m_size = sizeof(TValue) * count;

	return reinterpret_cast<TValue *>(m_pBlock);
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

inline byte_t const* HeapBlock::operator[](size_t index) const
{
	return Get(index);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline byte_t * HeapBlock::operator[](size_t index)
{
	return Get(index);
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
