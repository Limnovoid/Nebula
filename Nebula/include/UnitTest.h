#ifndef NEBULA_UNIT_TEST_H
#define NEBULA_UNIT_TEST_H

#include "Result.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

template<typename TReturn, typename TParameters>
class UnitTest : public IUnitTest<TReturn, TParameters>
{
public:
	using IUnitTest = IUnitTest<TReturn, TParameters>;

	UnitTest(UnitTest const& other);
	UnitTest(std::function<TReturn(TParameters)> const& func, StringView title = "");
	virtual ~UnitTest() override;

	virtual Result Invoke(TParameters const& parameters, TReturn & returned) override;

private:
	using Func = std::function<TReturn(TParameters)>;			// The unit test function type alias.

	Func const&						m_func;						// Reference to the unit test function.
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
UnitTest<TReturn, TParameters>::UnitTest(UnitTest const& other) :
	IUnitTest(other.GetTitle()),
	m_func(other.m_func)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
UnitTest<TReturn, TParameters>::UnitTest(std::function<TReturn(TParameters)> const& func, StringView title) :
	IUnitTest(title),
	m_func(func)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
UnitTest<TReturn, TParameters>::~UnitTest()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
Result UnitTest<TReturn, TParameters>::Invoke(TParameters const& parameters, TReturn & returned)
{
	returned = m_func(parameters); // Leave exception handling to the TestHandler.

	return RESULT_CODE_SUCCESS;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_UNIT_TEST_H

