#include "TestResult.h"

namespace Nebula
{

namespace Orion
{

TestResult::TestResult() :
	m_result(false),
	m_message("")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

TestResult::TestResult(bool const result, StringView const message) :
	m_result(result),
	m_message(message)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

TestResult::TestResult(bool const result, String && message) :
	m_result(result),
	m_message(std::move(message))
{
}

} // namespace Orion --------------------------------------------------------------------------------------------------------------

String ToString(Orion::TestResult const& testResult)
{
	static constexpr char FORMAT[] = "{"/*result*/"} \"{"/*message*/"}\"";

	return std::format(FORMAT, ToString(testResult.GetResult()), testResult.GetMessage());
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
