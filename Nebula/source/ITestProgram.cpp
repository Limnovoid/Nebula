#include "ITestProgram.h"

#include "Test.h"

namespace Nebula
{

ITestProgram::ITestProgram(StringView title) : m_title(title)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ITestProgram::~ITestProgram()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void ITestProgram::Run(TestHandler & testHandler)
{
	RunImpl(testHandler);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
