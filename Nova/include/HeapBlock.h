#ifndef NOVA_HEAP_BLOCK_H
#define NOVA_HEAP_BLOCK_H

#include "NebulaTypes.h"
#include "Footprint.h"
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
	HeapBlock(Footprint const& footprint);
	~HeapBlock();

	/// <returns> Whether the object has a reference to an existing block of memory. </returns>
	bool IsInitialized() const;

	/// <returns> The size of the allocated block as a number of TValues = total bytes / sizeof(T), or 0 if not initialized. </summary>
	template<typename T = byte_t>
	size_t Size() const;

	/// <summary>
	/// Get the address at a given index. Indices are spaced according to the size of T.
	/// Note: does *not* prevent indexing beyond the end of the block.
	/// </summary>
	/// <exception cref="AssertionException">
	/// Not Initialized - no memory block to index.
	/// Invalid Parameter - alignment mistmatch between T and the allocated block.
	/// </exception>
	template<typename T = byte_t>
	T const* Get(size_t index = 0) const;

	/// <summary>
	/// Get the address at a given index. Indices are spaced according to the size of T.
	/// Note: does *not* prevent indexing beyond the end of the block.
	/// </summary>
	/// <exception cref="AssertionException">
	/// Not Initialized - no memory block to index.
	/// Invalid Parameter - alignment mistmatch between T and the allocated block.
	/// </exception>
	template<typename T = byte_t>
	T * Get(size_t index = 0);

	/// <summary>
	/// Initialize the object with a contiguous block of memory from the heap.
	/// The allocated block is aligned to type T.
	/// </summary>
	/// <typeparam name="T">The type to use as a basis for allocating the block from the heap.</typeparam>
	/// <param name="footprint"> The footprint of the block to allocate. </param>
	/// <exception cref="AssertionException"> Already Initialized - a block is already allocated. </exception>
	template<typename T = byte_t>
	T * Initialize(Footprint footprint);

	/// <summary> Free the held block of memory. </summary>
	/// <exception cref="AssertionException"> Not Initialized - no block to free. </exception>
	void Release();

	/// <summary> Resize the block. Frees the existing block (if initialized) and acquires a new block from the heap. </summary>
	template<typename T = byte_t>
	T * Resize(Footprint footprint);

	byte_t const* operator[](size_t index) const;
	byte_t * operator[](size_t index);

private:
	static byte_t * AllocateBlock(Footprint footprint);

	static void FreeBlock(byte_t * pBlock);

	Footprint	m_footprint;
	byte_t *	m_pBlock;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline HeapBlock::HeapBlock(Footprint const& footprint) :
	m_footprint(footprint),
	m_pBlock(AllocateBlock(footprint))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T const* HeapBlock::Get(size_t index) const
{
	EXCEPTION_ASSERT(IsInitialized(), RESULT_CODE_NOT_INITIALIZED, "");
	EXCEPTION_ASSERT(0 == (reinterpret_cast<uintptr_t>(m_pBlock) % alignof(T)), RESULT_CODE_INVALID_PARAMETER, "");

	return reinterpret_cast<T const*>(m_pBlock) + index;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T * HeapBlock::Get(size_t index)
{
	EXCEPTION_ASSERT(IsInitialized(), RESULT_CODE_NOT_INITIALIZED, "");
	EXCEPTION_ASSERT(0 == (reinterpret_cast<uintptr_t>(m_pBlock) % alignof(T)), RESULT_CODE_INVALID_PARAMETER, "");

	return reinterpret_cast<T *>(m_pBlock) + index;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T * HeapBlock::Initialize(Footprint footprint)
{
	EXCEPTION_ASSERT(!IsInitialized(), RESULT_CODE_ALREADY_INITIALIZED, "");

	m_pBlock = AllocateBlock(footprint);
	m_footprint = footprint;

	return reinterpret_cast<T *>(m_pBlock);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T * HeapBlock::Resize(Footprint footprint)
{
	EXCEPTION_ASSERT(IsInitialized(), RESULT_CODE_NOT_INITIALIZED, "");

	FreeBlock(m_pBlock);

	m_pBlock = AllocateBlock(footprint);
	m_footprint = footprint;

	return reinterpret_cast<T *>(m_pBlock);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool HeapBlock::IsInitialized() const
{
	ASSERT((nullptr != m_pBlock) == (0 < m_footprint.Size()));

	return (nullptr != m_pBlock);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline size_t HeapBlock::Size() const
{
	if (0 == m_footprint.Size())
		return 0;

	return m_footprint.Size() / sizeof(T);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline byte_t * HeapBlock::AllocateBlock(Footprint footprint)
{
	return reinterpret_cast<byte_t *>(_aligned_malloc(footprint.Size(), footprint.Alignment()));
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
