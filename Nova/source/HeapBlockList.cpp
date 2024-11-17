#include "HeapBlockList.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

HeapBlockList::HeapBlockList() :
	m_head(),
	m_pTail(&m_head),
	m_length(1),
	m_nodeFootprint()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockList::HeapBlockList(Footprint const& headFootprint, size_t nodeFootprintCount) :
	m_head(headFootprint),
	m_pTail(&m_head),
	m_length(1),
	m_nodeFootprint((0 == nodeFootprintCount) ? headFootprint :
		Footprint(nodeFootprintCount, headFootprint.TypeSize(), headFootprint.Alignment()))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockList::~HeapBlockList()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockList::Iterator HeapBlockList::Extend(size_t numNodes)
{
	Node * pFirstNewNode = nullptr;

	m_length += numNodes;

	while (0 < numNodes)
	{
		--numNodes;

		assert(!(m_pTail->m_pNext));

		SharedPtr<Node> pNewNode = MakeShared<Node>(m_nodeFootprint);

		m_pTail->m_pNext = pNewNode;
		m_pTail = pNewNode.get();

		if (nullptr == pFirstNewNode)
			pFirstNewNode = pNewNode.get();
	}

	return Iterator(pFirstNewNode);
}

// --------------------------------------------------------------------------------------------------------------------------------

void HeapBlockList::Shorten(Iterator pNewTail)
{

}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockList::Node::Node() :
	HeapBlock(),
	m_pNext(nullptr)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockList::Node::Node(Footprint const& footprint) :
	HeapBlock(footprint),
	m_pNext(nullptr)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockList::Node::Node(Footprint const& footprint, SharedPtr<Node> pNext) :
	HeapBlock(footprint),
	m_pNext(pNext)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockListTestProgram::HeapBlockListTestProgram() :
	ITestProgram("HeapBlockList")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

HeapBlockListTestProgram::~HeapBlockListTestProgram()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void HeapBlockListTestProgram::RunImpl(TestHandler & testHandler)
{
	using Node = HeapBlockList::Node;

	HeapBlockList heapBlockList;

	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlockList.m_head.Size(); }, 0, 0, "Uninitialized head size");
	testHandler.Assert<uintptr_t, int>([&](int) -> size_t { return reinterpret_cast<uintptr_t>(heapBlockList.m_pTail); },
		0, reinterpret_cast<uintptr_t>(&heapBlockList.m_head), "Uninitialized tail points to head");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return heapBlockList.m_length; }, 0, 1, "Uninitialized length");
	testHandler.Assert<bool, int>([&](int) -> bool { return heapBlockList.m_nodeFootprint.IsInitialized(); }, 0, false, "Uninitialized node footprint");

}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
