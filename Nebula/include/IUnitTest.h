#ifndef NEBULA_I_UNIT_TEST_H
#define NEBULA_I_UNIT_TEST_H

#include "Result.h"
#include "NebulaString.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

template<typename TReturn, typename TParameters>
class IUnitTest
{
public:
	IUnitTest(StringView title);

	virtual ~IUnitTest();

	StringView GetTitle();

	virtual Result Invoke(TParameters const& parameters, TReturn & returned) = 0;

private:
	String	m_title;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline IUnitTest<TReturn, TParameters>::IUnitTest(StringView title) :
	m_title(title)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline IUnitTest<TReturn, TParameters>::~IUnitTest()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TReturn, typename TParameters>
inline StringView IUnitTest<TReturn, TParameters>::GetTitle()
{
	return MakeStringView(m_title);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_I_UNIT_TEST_H
