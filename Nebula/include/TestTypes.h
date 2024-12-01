#ifndef NEBULA_TEST_TYPES_H
#define NEBULA_TEST_TYPES_H

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

template<typename T>
class LifetimeTracker
{
public:
	template<typename... TArgs>
	LifetimeTracker(TArgs... args) :
		m_value(std::forward<TArgs>(args)...)
	{
		++s_nInstances;
	}

	~LifetimeTracker()
	{
		--s_nInstances;
	}

	static size_t GetNumInstances()
	{
		return s_nInstances;
	}

	T	m_value;

private:
	inline static size_t	s_nInstances = 0;
};

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TEST_TYPES_H
