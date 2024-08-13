#ifndef NEBULA_ORION_TEST_RESULT_H
#define NEBULA_ORION_TEST_RESULT_H

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
	TestResult(bool const result, std::string const& message);
	TestResult(bool const result, std::string && message);

	void SetResult(bool const result);
	void SetMessage(std::string const& message);
	void SetMessage(std::string && message);

	bool GetResult() const;
	std::string_view GetMessage() const;

private:
	bool			m_result;
	std::string		m_message;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline void TestResult::SetResult(bool const result)
{
	m_result = result;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void TestResult::SetMessage(std::string const& message)
{
	m_message = message;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void TestResult::SetMessage(std::string && message)
{
	m_message = std::move(message);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool TestResult::GetResult() const
{
	return m_result;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline std::string_view TestResult::GetMessage() const
{
	return std::string_view(m_message.data(), m_message.size());
}

} // namespace Orion --------------------------------------------------------------------------------------------------------------

std::string ToString(Orion::TestResult const& testResult);

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_ORION_TEST_RESULT_H
