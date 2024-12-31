#ifndef NEBULA_TEST_TYPES_H
#define NEBULA_TEST_TYPES_H

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

template<typename T>
class LifetimeTracker
{
public:
	static size_t GetNumInstances()
	{
		return s_nInstances;
	}

	template<typename... TArgs>
	LifetimeTracker(TArgs... args) :
		m_value(std::forward<TArgs>(args)...)
	{
		++s_nInstances;
	}

	LifetimeTracker(LifetimeTracker const& other) :
		m_value(other.m_value)
	{
		++s_nInstances;
	}

	LifetimeTracker(LifetimeTracker && other) :
		m_value(std::move(other.m_value))
	{
		++s_nInstances;
	}

	~LifetimeTracker()
	{
		--s_nInstances;
	}

	bool operator<(LifetimeTracker const& rhs) const
	{
		return m_value < rhs.m_value;
	}

	bool operator==(LifetimeTracker const& rhs) const
	{
		return m_value == rhs.m_value;
	}

	LifetimeTracker & operator=(LifetimeTracker const& rhs)
	{
		m_value = rhs.m_value;

		return *this;
	}

	LifetimeTracker & operator=(LifetimeTracker && rhs)
	{
		m_value = std::move(rhs.m_value);

		return *this;
	}

	T	m_value;

private:
	inline static size_t	s_nInstances = 0;
};

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TEST_TYPES_H
