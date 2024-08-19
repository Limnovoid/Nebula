#ifndef NEBULA_ORION_TEST_RESULT_H
#define NEBULA_ORION_TEST_RESULT_H

#include "NebulaString.h"

namespace Nebula
{

namespace Orion
{

class TestResult
{
public:
	TestResult();
	TestResult(TestResult const&) = default;
	TestResult(TestResult &&) = default;
	TestResult(bool const result, StringView const message);
	TestResult(bool const result, String && message);

	void SetResult(bool const result);
	void SetMessage(StringView const message);
	void SetMessage(String && message);

	bool GetResult() const;
	StringView GetMessage() const;

private:
	bool		m_result;
	String		m_message;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline void TestResult::SetResult(bool const result)
{
	m_result = result;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void TestResult::SetMessage(StringView const message)
{
	m_message = message;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void TestResult::SetMessage(String && message)
{
	m_message = std::move(message);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool TestResult::GetResult() const
{
	return m_result;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView TestResult::GetMessage() const
{
	return MakeStringView(m_message);
}

} // namespace Orion --------------------------------------------------------------------------------------------------------------

String ToString(Orion::TestResult const& testResult);

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_ORION_TEST_RESULT_H
