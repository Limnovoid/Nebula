#ifndef NOVA_SORTED_LIST_H
#define NOVA_SORTED_LIST_H

#include "NebulaTypes.h"
#include "ITestScript.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

/// <summary>
/// Simple sorted container type implemented as a linked list.
/// O(n) insertion.
/// </summary>
/// <typeparam name="T"> The type to store. </typeparam>
/// <typeparam name="TPredicate"> The sorting predicate. Binary operator which returns true iff the LHS operand should be placed before the RHS operand in the sorted list. </typeparam>
template<typename T, typename TPredicate = std::less<T>>
class SortedList : std::list<T>
{
	using Base = std::list<T>;

public:
	using iterator = Base::iterator;
	using const_iterator = Base::const_iterator;
	using Base::list;
	using Base::begin;
	using Base::end;
	using Base::cbegin;
	using Base::cend;
	using Base::size;

	iterator Insert(T const& value);

private:
	TPredicate		m_predicate;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate>
SortedList<T, TPredicate>::iterator SortedList<T, TPredicate>::Insert(T const& value)
{
	const_iterator iterNext = cbegin();

	while ((cend() != iterNext) && !m_predicate(value, *iterNext))
		++iterNext;

	return Base::insert(iterNext, value);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class SortedListTestScript : public Nebula::ITestScript
{
public:
	SortedListTestScript();
	virtual ~SortedListTestScript();

protected:
	virtual void RunImpl(Nebula::TestHandler & testHandler) override;
};

} // namespace Nova ---------------------------------------------------------------------------------------------------------------

#endif//NOVA_SORTED_LIST_H
