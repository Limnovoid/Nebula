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

	template<typename... TArgs>
	iterator Emplace(TArgs &&... args);

	void Erase(iterator pos);

	const_iterator Find(T const& value) const;

private:
	const_iterator FindSortedPos(T const& value) const;

	static TPredicate		s_predicate;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate>
TPredicate SortedList<T, TPredicate>::s_predicate = {};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate>
SortedList<T, TPredicate>::iterator SortedList<T, TPredicate>::Insert(T const& value)
{
	const_iterator pos = FindSortedPos(value);

	return Base::insert(pos, value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate>
template<typename... TArgs>
SortedList<T, TPredicate>::iterator SortedList<T, TPredicate>::Emplace(TArgs &&... args)
{
	T value(std::forward<TArgs>(args)...);

	const_iterator pos = FindSortedPos(value);

	return Base::emplace(pos, std::move(value));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate>
void SortedList<T, TPredicate>::Erase(iterator pos)
{
	Base::erase(pos);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate>
SortedList<T, TPredicate>::const_iterator SortedList<T, TPredicate>::Find(T const& value) const
{
	const_iterator pos = FindSortedPos(value);

	// FindSortedPos returns the first position for which (pos < value) is false:
	// we test that (value < pos) is also false to confirm they are equal, and if not we return cend().
	if ((cend() != pos) && s_predicate(value, *pos))
		pos = cend();

	return pos;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate>
SortedList<T, TPredicate>::const_iterator SortedList<T, TPredicate>::FindSortedPos(T const& value) const
{
	const_iterator pos = cbegin();

	while ((cend() != pos) && s_predicate(*pos, value))
		++pos;

	return pos;
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
