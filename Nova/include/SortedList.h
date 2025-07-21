#ifndef NOVA_SORTED_LIST_H
#define NOVA_SORTED_LIST_H

#include "NebulaTypes.h"
#include "ITestScript.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

/// <summary> Simple sorted container type. </summary>
/// <typeparam name="T"> The type to store. </typeparam>
/// <typeparam name="TPredicate"> The sorting predicate. Must be a binary operator which returns true iff the LHS operand should be placed before the RHS operand in the sorted list. </typeparam>
template<typename T, typename TPredicate = std::less<T>, template <typename> class TContainer = std::list>
class SortedList
{
	using Container = TContainer<T>;

public:
	using Iterator = Container::iterator;
	using ConstIterator = Container::const_iterator;

	bool Empty() const;
	size_t Size() const;

	Iterator Begin();
	ConstIterator CBegin() const;
	Iterator End();
	ConstIterator CEnd() const;

	T & Front();
	T const& Front() const;
	T & Back();
	T const& Back() const;

	/// <summary> Copy insert. O(n) complexity. </summary>
	/// <param name="value"> The value to copy into the insert element. </param>
	/// <returns> An iterator to the insert element. </returns>
	Iterator Insert(T const& value);

	/// <summary> Construct an element in place. O(n) complexity. </summary>
	/// <param name="...args"> The constructor arguments. </param>
	/// <returns> An iterator to the emplaced element. </returns>
	template<typename... TArgs>
	Iterator Emplace(TArgs &&... args);

	/// <summary> Erase the element at the given position. </summary>
	/// <param name="pos"> The position of the element to erase. </param>
	void Erase(Iterator pos);

	/// <summary> Find an element by value. </summary>
	/// <param name="value"> The value of the element to find. </param>
	/// <returns> An iterator to the element, or the end iterator if not found. </returns>
	Iterator Find(T const& value);

	/// <summary> Find an element by value. </summary>
	/// <param name="value"> The value of the element to find. </param>
	/// <returns> A const iterator to the element, or the const end iterator if not found. </returns>
	ConstIterator Find(T const& value) const;

	/// <summary> Sort a particular element relative to its neighbours. </summary>
	/// <param name="pos"> The position of the element to sort. </param>
	Iterator Sort(Iterator pos);

protected:
	/// <summary> Search forwards from 'pos' for the sorted position of the given value. </summary>
	/// <param name="value"> The value to sort. </param>
	/// <param name="pos"> The position of the first list item to compare against. </param>
	/// <returns> The sorted position of the given value. </returns>
	Iterator FindSortedPosForwards(T const& value, Iterator pos);

	/// <summary> Search backwards from 'pos' for the sorted position of the given value. </summary>
	/// <param name="value"> The value to sort. </param>
	/// <param name="pos"> The position of the first list item to compare against. </param>
	/// <returns> The sorted position of the given value. </returns>
	Iterator FindSortedPosBackwards(T const& value, Iterator pos);

	static TPredicate	s_predicate;

	Container			m_container;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
TPredicate SortedList<T, TPredicate, TContainer>::s_predicate = {};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
bool SortedList<T, TPredicate, TContainer>::Empty() const
{
	return m_container.empty();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
size_t SortedList<T, TPredicate, TContainer>::Size() const
{
	return m_container.size();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::Iterator SortedList<T, TPredicate, TContainer>::Begin()
{
	return m_container.begin();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::ConstIterator SortedList<T, TPredicate, TContainer>::CBegin() const
{
	return m_container.cbegin();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::Iterator SortedList<T, TPredicate, TContainer>::End()
{
	return m_container.end();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::ConstIterator SortedList<T, TPredicate, TContainer>::CEnd() const
{
	return m_container.cend();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
T & SortedList<T, TPredicate, TContainer>::Front()
{
	return m_container.front();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
T const& SortedList<T, TPredicate, TContainer>::Front() const
{
	return m_container.front();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
T & SortedList<T, TPredicate, TContainer>::Back()
{
	return m_container.back();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
T const& SortedList<T, TPredicate, TContainer>::Back() const
{
	return m_container.back();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::Iterator SortedList<T, TPredicate, TContainer>::Insert(T const& value)
{
	Iterator pos = FindSortedPosForwards(value, m_container.begin());

	return m_container.insert(pos, value);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
template<typename... TArgs>
SortedList<T, TPredicate, TContainer>::Iterator SortedList<T, TPredicate, TContainer>::Emplace(TArgs &&... args)
{
	T value(std::forward<TArgs>(args)...);

	Iterator pos = FindSortedPosForwards(value, m_container.begin());

	return m_container.emplace(pos, std::move(value));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
void SortedList<T, TPredicate, TContainer>::Erase(Iterator pos)
{
	m_container.erase(pos);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::Iterator SortedList<T, TPredicate, TContainer>::Find(T const& value)
{
	Iterator pos = FindSortedPosForwards(value, m_container.begin());

	// FindSortedPosForwards returns the first position for which (pos < value) is false:
	// we test that (value < pos) is also false to confirm they are equal, and if not we return end().
	if ((m_container.end() != pos) && s_predicate(value, *pos))
		pos = m_container.end();

	return pos;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::ConstIterator SortedList<T, TPredicate, TContainer>::Find(T const& value) const
{
	ConstIterator pos = FindSortedPosForwards(value, m_container.begin());

	// FindSortedPos returns the first position for which (pos < value) is false:
	// we test that (value < pos) is also false to confirm they are equal, and if not we return cend().
	if ((m_container.cend() != pos) && s_predicate(value, *pos))
		pos = m_container.cend();

	return pos;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::Iterator SortedList<T, TPredicate, TContainer>::Sort(Iterator pos)
{
	Iterator newPos = m_container.end();

	if (m_container.end() != pos)
	{
		Iterator nextPos = pos;
		++nextPos;

		if ((m_container.end() != nextPos) && s_predicate(*nextPos, *pos))
			newPos = FindSortedPosForwards(*pos, nextPos);
	}

	if ((m_container.end() == newPos) && (pos != m_container.begin()))
	{
		Iterator prevPos = pos;
		--prevPos;

		if (s_predicate(*pos, *prevPos))
			newPos = FindSortedPosBackwards(*pos, prevPos);
	}

	newPos = m_container.emplace(newPos, std::move(*pos));
	m_container.erase(pos);

	return newPos;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::Iterator SortedList<T, TPredicate, TContainer>::FindSortedPosForwards(T const& value, Iterator pos)
{
	while ((m_container.end() != pos) && s_predicate(*pos, value))
		++pos;

	return pos;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename TPredicate, template <typename> class TContainer>
SortedList<T, TPredicate, TContainer>::Iterator SortedList<T, TPredicate, TContainer>::FindSortedPosBackwards(T const& value, Iterator pos)
{
	if (m_container.empty())
		return m_container.end();

	if (m_container.end() == pos)
		--pos;

	do
	{
		if (s_predicate(value, *pos))
			break;

		--pos;
	}
	while (m_container.begin() != pos);

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
