#ifndef NOVA_HEAP_BLOCK_LIST_H
#define NOVA_HEAP_BLOCK_LIST_H

#include "HeapBlock.h"
#include "TestHandler.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

class HeapBlockList
{
	friend class HeapBlockListTestScript;

public:
	class Node : public HeapBlock
	{
		friend class HeapBlockList;

	public:
		Node();
		Node(Footprint const& footprint);

	private:
		Node(Footprint const& footprint, SharedPtr<Node> pNext);

		SharedPtr<Node>		m_pNext;				// Pointer to the next node in the list.
	};

	class Iterator
	{
		friend class HeapBlockList;

	public:
		using difference_type = std::ptrdiff_t;
		using value_type = Node;
		using pointer = value_type *;
		using reference = value_type &;

		Iterator(Iterator const& rhs);

		Iterator & operator++();
		Iterator operator++(int);

		bool operator==(Iterator const& rhs) const;
		bool operator!=(Iterator const& rhs) const;

		Node & operator*();
		Node * operator->();

	private:
		Iterator(Node * pNode);

		Node *			m_pNode;
	};

	class ConstIterator
	{
		friend class HeapBlockList;

	public:
		using difference_type = std::ptrdiff_t;
		using value_type = Node;
		using pointer = value_type const*;
		using reference = value_type const&;

		ConstIterator(Iterator const& rhs);
		ConstIterator(ConstIterator const& rhs);

		ConstIterator & operator++();
		ConstIterator operator++(int);

		bool operator==(ConstIterator const& rhs) const;
		bool operator!=(ConstIterator const& rhs) const;

		Node const& operator*() const;
		Node const* operator->() const;

	private:
		ConstIterator(Node const* pNode);

		Node const*		m_pNode;
	};

	HeapBlockList();
	HeapBlockList(Footprint const& NodeFootprint, size_t nodeFootprintCount = 0);
	~HeapBlockList();

	Iterator GetHead();
	Iterator GetTail();
	ConstIterator GetHead() const;
	ConstIterator GetTail() const;

	Iterator begin();
	Iterator end();
	ConstIterator cbegin() const;
	ConstIterator cend() const;

	/// <summary> Create and add a new sequence of nodes to the end of the list. The list's tail pointer is set to the last node in this new sequence. </summary>
	/// <param name="numNodes"> The number of nodes to create. </param>
	/// <returns> Pointer to the first new node. </returns>
	Iterator Extend(size_t numNodes = 1);

	/// <summary> Delete the block list after the given node. The given node becomes the new tail. </summary>
	/// <param name="pNewTail"> The node to be the last in the list after the delete. Defaults to the Node node (deletes the entire list except the Node). </param>
	void Shorten(Iterator newTail);

private:
	Node			m_head;				// The heap block list Node.
	Node *			m_pTail;			// Pointer to the tail.
	size_t			m_length;			// The total number of nodes (including the Node).
	Footprint		m_nodeFootprint;	// The node footprint.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Iterator HeapBlockList::GetHead()
{
	return &m_head;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Iterator HeapBlockList::GetTail()
{
	return m_pTail;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::ConstIterator HeapBlockList::GetHead() const
{
	return &m_head;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::ConstIterator HeapBlockList::GetTail() const
{
	return m_pTail;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Iterator HeapBlockList::begin()
{
	return GetHead();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Iterator HeapBlockList::end()
{
	return GetTail();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::ConstIterator HeapBlockList::cbegin() const
{
	return GetHead();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::ConstIterator HeapBlockList::cend() const
{
	return GetTail();
}

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Iterator::Iterator(Iterator const& rhs) :
	m_pNode(rhs.m_pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Iterator::Iterator(Node * pNode) :
	m_pNode(pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Iterator & HeapBlockList::Iterator::operator++()
{
	m_pNode = m_pNode->m_pNext.get();

	return (*this);
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Iterator HeapBlockList::Iterator::operator++(int)
{
	Iterator retval = *this;

	++(*this);

	return retval;
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline bool HeapBlockList::Iterator::operator==(Iterator const& rhs) const
{
	return (m_pNode == rhs.m_pNode);
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline bool HeapBlockList::Iterator::operator!=(Iterator const& rhs) const
{
	return (m_pNode != rhs.m_pNode);
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Node & HeapBlockList::Iterator::operator*()
{
	return *m_pNode;
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Node * HeapBlockList::Iterator::operator->()
{
	return m_pNode;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::ConstIterator::ConstIterator(Iterator const& rhs) :
	m_pNode(rhs.m_pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::ConstIterator::ConstIterator(ConstIterator const& rhs) :
	m_pNode(rhs.m_pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::ConstIterator::ConstIterator(Node const* pNode) :
	m_pNode(pNode)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::ConstIterator & HeapBlockList::ConstIterator::operator++()
{
	m_pNode = m_pNode->m_pNext.get();

	return (*this);
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::ConstIterator HeapBlockList::ConstIterator::operator++(int)
{
	ConstIterator retval = *this;

	++(*this);

	return retval;
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline bool HeapBlockList::ConstIterator::operator==(ConstIterator const& rhs) const
{
	return (m_pNode == rhs.m_pNode);
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline bool HeapBlockList::ConstIterator::operator!=(ConstIterator const& rhs) const
{
	return (m_pNode != rhs.m_pNode);
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Node const& HeapBlockList::ConstIterator::operator*() const
{
	return *m_pNode;
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline HeapBlockList::Node const* HeapBlockList::ConstIterator::operator->() const
{
	return m_pNode;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class HeapBlockListTestScript : public ITestScript
{
public:
	HeapBlockListTestScript();
	virtual ~HeapBlockListTestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Nova ---------------------------------------------------------------------------------------------------------------

#endif//NOVA_HEAP_BLOCK_LIST_H
