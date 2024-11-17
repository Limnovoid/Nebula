#ifndef NOVA_ANCHORED_LIST_H
#define NOVA_ANCHORED_LIST_H

#include "NebulaTypes.h"
#include "TestHandler.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

/// <summary>
/// A container class implemented as a linked list in which the head of the list is a class member;
/// i.e., the length of the list is always at least 1.
/// </summary>
/// <typeparam name="T"> The type stored by the list. </typeparam>
template<typename T>
class AnchoredList
{
	friend class AnchoredListTestProgram;

public:
	class Node
	{
		friend class AnchoredList;
		friend class AnchoredListTestProgram;

	public:
		template<typename... TArgs>
		Node(TArgs... valueArgs);

	private:
		SharedPtr<Node>		m_pNext;				// Pointer to the next node in the list.

		T					m_value;				// The node value.
	};

	class Iterator
	{
		friend class AnchoredList;

	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = value_type *;
		using reference = value_type &;

		Iterator(Iterator const& rhs);

		Iterator & operator++();
		Iterator operator++(int);

		bool operator==(Iterator const& rhs) const;
		bool operator!=(Iterator const& rhs) const;

		T & operator*();
		T * operator->();

	private:
		Iterator(Node * pNode);

		Node *			m_pNode;
	};

	class ConstIterator
	{
		friend class AnchoredList;

	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = value_type const*;
		using reference = value_type const&;

		ConstIterator(Iterator const& rhs);
		ConstIterator(ConstIterator const& rhs);

		ConstIterator & operator++();
		ConstIterator operator++(int);

		bool operator==(ConstIterator const& rhs) const;
		bool operator!=(ConstIterator const& rhs) const;

		T const& operator*() const;
		T const* operator->() const;

	private:
		ConstIterator(Node const* pNode);

		Node const*		m_pNode;
	};

	template<typename... TArgs>
	AnchoredList(TArgs... baseValueArgs);

	Iterator begin();
	Iterator end();
	ConstIterator cbegin() const;
	ConstIterator cend() const;

	template<typename... TArgs>
	Iterator EmplaceBack(TArgs... valueArgs);

private:
	Node			m_head;				// The heap block list Node.
	Node *			m_pTail;			// Pointer to the tail.
	size_t			m_length;			// The total number of nodes (including the Node).
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
template<typename... TArgs>
inline AnchoredList<T>::AnchoredList(TArgs... baseValueArgs) :
	m_head(std::forward<TArgs>(baseValueArgs)...),
	m_pTail(&m_head),
	m_length(1)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::Iterator AnchoredList<T>::begin()
{
	return Iterator(&m_head);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::Iterator AnchoredList<T>::end()
{
	return Iterator(m_pTail);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::ConstIterator AnchoredList<T>::cbegin() const
{
	return ConstIterator(&m_head);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::ConstIterator AnchoredList<T>::cend() const
{
	return ConstIterator(m_pTail);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
template<typename... TArgs>
inline AnchoredList<T>::Iterator AnchoredList<T>::EmplaceBack(TArgs... valueArgs)
{
	assert(!m_pTail->m_pNext);

	m_pTail->m_pNext = MakeShared<Node>(std::forward<TArgs>(valueArgs)...);

	m_pTail = m_pTail->m_pNext;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
template<typename... TArgs>
inline AnchoredList<T>::Node::Node(TArgs... valueArgs) :
	m_value(std::forward<TArgs>(valueArgs)...)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline AnchoredList<T>::Iterator::Iterator(Iterator const& rhs) :
	m_pNode(rhs.m_pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::Iterator::Iterator(Node * pNode) :
	m_pNode(pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::Iterator & AnchoredList<T>::Iterator::operator++()
{
	m_pNode = m_pNode->m_pNext.get();

	return (*this);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::Iterator AnchoredList<T>::Iterator::operator++(int)
{
	Iterator retval = *this;

	++(*this);

	return retval;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline bool AnchoredList<T>::Iterator::operator==(Iterator const& rhs) const
{
	return (m_pNode == rhs.m_pNode);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline bool AnchoredList<T>::Iterator::operator!=(Iterator const& rhs) const
{
	return (m_pNode != rhs.m_pNode);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline T & AnchoredList<T>::Iterator::operator*()
{
	return m_pNode->m_value;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline T * AnchoredList<T>::Iterator::operator->()
{
	return &m_pNode->m_value;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::ConstIterator::ConstIterator(Iterator const& rhs) :
	m_pNode(rhs.m_pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::ConstIterator::ConstIterator(ConstIterator const& rhs) :
	m_pNode(rhs.m_pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::ConstIterator::ConstIterator(Node const* pNode) :
	m_pNode(pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::ConstIterator & AnchoredList<T>::ConstIterator::operator++()
{
	m_pNode = m_pNode->m_pNext.get();

	return (*this);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline AnchoredList<T>::ConstIterator AnchoredList<T>::ConstIterator::operator++(int)
{
	ConstIterator retval = *this;

	++(*this);

	return retval;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline bool AnchoredList<T>::ConstIterator::operator==(ConstIterator const& rhs) const
{
	return (m_pNode == rhs.m_pNode);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline bool AnchoredList<T>::ConstIterator::operator!=(ConstIterator const& rhs) const
{
	return (m_pNode != rhs.m_pNode);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline T const& AnchoredList<T>::ConstIterator::operator*() const
{
	return m_pNode->m_value;
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>inline T const* AnchoredList<T>::ConstIterator::operator->() const
{
	return &m_pNode->m_value;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class AnchoredListTestProgram : public ITestProgram
{
public:
	AnchoredListTestProgram();
	virtual ~AnchoredListTestProgram();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Nova ---------------------------------------------------------------------------------------------------------------

#endif//NOVA_ANCHORED_LIST_H
