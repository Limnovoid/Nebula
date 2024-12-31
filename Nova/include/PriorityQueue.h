#ifndef NOVA_PRIORITY_QUEUE_H
#define NOVA_PRIORITY_QUEUE_H

#include "NebulaTypes.h"
#include "TestHandler.h"
#include "Macros.h"

namespace Nova // ---------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

/// <summary>
/// Priority queue container type implemented as an array-based heap tree.
/// Structured such that, for any node N and child C, comparison functor F and equality functor E:
/// either F(N, C) is true or E(N, C) is true.
/// Minimising by default - use TCompareFunctor = std::greater for maximising.
/// </summary>
/// <typeparam name="T"> The type to store in the queue. </typeparam>
/// <typeparam name="TCompareFunctor"> The type of the comparison functor. </typeparam>
template<typename T, IsCompareFunctor<T> TCompareFunctor = std::less<T>, IsCompareFunctor<T> TEqualsFunctor = std::equal_to<T>>
class PriorityQueue
{
public:
	/// <summary> Default constructor. </summary>
	PriorityQueue();

	/// <summary> Vector constructor. </summary>
	/// <param name="...args"> The arguments to pass to the underlying vector. </param>
	template<typename... TArgs>
	PriorityQueue(TArgs... args);

	/// <summary> Copy an element into its sorted position in the heap. </summary>
	void Insert(T const& t);

	/// <summary> Construct an element at its sorted position in the heap. </summary>
	template<typename... TArgs>
	void Emplace(TArgs... args);

	/// <summary> Remove the first element from the front of the queue. </summary>
	void Pop();

	/// <summary>
	/// Sort the first element in the queue. Assumes all other elements already satisfy the heap structure constraints.
	/// Useful for modifying the root element without having to remove and re-insert.
	/// </summary>
	void SortFront();

	size_t Size() const;
	bool Empty() const;

	// Get the front of the queue.
	T & Front();
	T const& Front() const;

private:
	void SortBack();

	size_t Parent(size_t index) const;
	size_t LeftChild(size_t index) const;
	size_t RightChild(size_t index) const;

	inline static const TCompareFunctor	s_compare = {};
	inline static const TEqualsFunctor	s_equals = {};

	std::vector<T>						m_array;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::PriorityQueue()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
template<typename... TArgs>
inline PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::PriorityQueue(TArgs... args) :
	m_array(std::forward<TArgs>(args)...)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline void PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::Insert(T const& t)
{
	m_array.push_back(t);

	SortBack();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
template<typename... TArgs>
inline void PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::Emplace(TArgs... args)
{
	m_array.emplace_back(std::forward<TArgs>(args)...);

	SortBack();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline void PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::Pop()
{
	ASSERT_THROW(0 < m_array.size(), RESULT_CODE_EMPTY, "Cannot Pop from an empty container.");

	size_t newSize = m_array.size() - 1;

	if (0 < newSize)
	{
		if constexpr (std::is_move_assignable_v<T>)
		{
			m_array.front() = std::move(m_array.back());
		}
		else
		{
			static_assert(std::is_copy_assignable_v<T>);

			m_array.front() = m_array.back();
		}
	}

	m_array.resize(newSize);

	if (1 < newSize)
		SortFront();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline void PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::SortFront()
{
	size_t heapIndex = 1;

	if (m_array.size() < heapIndex)
		return;

	while (true)
	{
		size_t const leftChildHeapIndex = LeftChild(heapIndex);
		size_t const rightChildHeapIndex = RightChild(heapIndex);

		size_t childHeapIndex;

		if (m_array.size() < leftChildHeapIndex) // Missing left child.
		{
			break; // Guaranteed to be missing both children - sorting is finished.
		}
		else if (m_array.size() < rightChildHeapIndex) // Missing right child.
		{
			childHeapIndex = leftChildHeapIndex; // Guaranteed to be the only child.
		}
		else
		{
			// Choose the child to swap with such that swapping will preserve the structure.
			childHeapIndex = s_compare(m_array[leftChildHeapIndex - 1], m_array[rightChildHeapIndex - 1]) ?
				leftChildHeapIndex : rightChildHeapIndex;
		}

		if (s_compare(m_array[heapIndex - 1], m_array[childHeapIndex - 1]) || s_equals(m_array[heapIndex - 1], m_array[childHeapIndex - 1]))
			break; // Elemented is sorted with respect to both children - sorting is finished.

		std::swap(m_array[heapIndex - 1], m_array[childHeapIndex - 1]);

		heapIndex = childHeapIndex;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline size_t PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::Size() const
{
	return m_array.size();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline bool PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::Empty() const
{
	return m_array.empty();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline T & PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::Front()
{
	return m_array.front();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline T const& PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::Front() const
{
	return m_array.front();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline void PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::SortBack()
{
	assert(0 < m_array.size());

	size_t heapIndex = m_array.size();

	while (1 < heapIndex)
	{
		size_t heapIndexParent = Parent(heapIndex);

		if (!s_compare(m_array[heapIndex - 1], m_array[heapIndexParent - 1]))
			break;

		std::swap(m_array[heapIndex - 1], m_array[heapIndexParent - 1]);

		heapIndex = heapIndexParent;
	};
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline size_t PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::Parent(size_t index) const
{
	assert(0 < index);

	return index / 2;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline size_t PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::LeftChild(size_t index) const
{
	return index * 2;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, IsCompareFunctor<T> TCompareFunctor, IsCompareFunctor<T> TEqualsFunctor>
inline size_t PriorityQueue<T, TCompareFunctor, TEqualsFunctor>::RightChild(size_t index) const
{
	return (index * 2) + 1;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class PriorityQueueTestScript : public ITestScript
{
public:
	PriorityQueueTestScript();
	virtual ~PriorityQueueTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Nova -------------------------------------------------------------------------------------------------------------

#endif//NOVA_PRIORITY_QUEUE_H
