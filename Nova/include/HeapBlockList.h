#ifndef NOVA_HEAP_BLOCK_LIST_H
#define NOVA_HEAP_BLOCK_LIST_H

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

class HeapBlockList
{
public:
	class Node;

	class Head : public HeapBlock
	{
	public:
		template<typename TValue>
		Head(BlockSize<TValue> const& size, Node * pNext = nullptr);

		Node *		m_pNext;				// Pointer to the next node in the list.
	};

	class Node : public Head
	{
	public:
		template<typename TValue>
		Node(BlockSize<TValue> const& size, Node * pPrev, Node * pNext = nullptr);

		Node *		m_pPrev;				// Pointer to the previous node in the list.
	};

	template<typename TValue>
	HeapBlockList(BlockSize<TValue> const& headSize, BlockSize<TValue> const& nodeSize = 0);
	~HeapBlockList();

private:
	Head		m_head;				// The heap block list head.

	size_t		m_nodeBlockSize;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
HeapBlockList::HeapBlockList(BlockSize<TValue> const& headSize, BlockSize<TValue> const& nodeSize) :
	m_head(headSize),
	m_nodeBlockSize(nodeSize.Get())
{
}

// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockList::~HeapBlockList()
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
HeapBlockList::Head::Head(BlockSize<TValue> const& size, Node * pNext) :
	HeapBlock(size),
	m_pNext(pNext)
{
}

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

template<typename TValue>
HeapBlockList::Node::Node(BlockSize<TValue> const& size, Node * pPrev, Node * pNext) :
	Head(size, pNext),
	m_pPrev(pPrev)
{
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------

#endif//NOVA_HEAP_BLOCK_LIST_H
