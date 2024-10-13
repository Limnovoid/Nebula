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

	UnitTest(std::function<TReturn(TParameters)> func, StringView title);
	virtual ~UnitTest() override;

	virtual Result Invoke(TParameters const& parameters, TReturn & returned) override;

private:
	using Func = std::function<TReturn(TParameters)>;

	Func	m_func;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
UnitTest<TReturn, TParameters>::UnitTest(std::function<TReturn(TParameters)> func, StringView title) :
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
	try
	{
		returned = m_func(parameters);
	}
	catch (AssertionException const& e)
	{
		return e.GetResultCode();
	}
	catch (Exception const& e)
	{
		return e.GetResultCode();
	}

	return RESULT_CODE_SUCCESS;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_UNIT_TEST_H

