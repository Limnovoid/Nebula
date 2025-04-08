#ifndef NEBULA_I_TEST_SCRIPT_H
#define NEBULA_I_TEST_SCRIPT_H

#include "NebulaString.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class TestHandler;

// --------------------------------------------------------------------------------------------------------------------------------

class ITestScript
{
public:
	ITestScript(StringView title);

	virtual ~ITestScript();

	void Run(TestHandler & testHandler);

	StringView GetTitle() const;

protected:
	virtual void RunImpl(TestHandler & testHandler) = 0;

	String		m_title;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView ITestScript::GetTitle() const
{
	return MakeStringView(m_title);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_I_TEST_SCRIPT_H
