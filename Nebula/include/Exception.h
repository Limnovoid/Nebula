#ifndef NEBULA_EXCEPTION_H
#define NEBULA_EXCEPTION_H

#include "Result.h"
#include "Format.h"
#include "ITestScript.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class Exception : std::exception
{
public:
	Exception(Result result, StringView message);

	Result GetResult() const;
	String const& GetMessage() const;

	String ToString() const;

private:
	Result		m_result;
	String		m_message;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline Result Exception::GetResult() const
{
	return m_result;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline String const& Exception::GetMessage() const
{
	return m_message;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline String Exception::ToString() const
{
	return Fmt::Format("{} - {}", m_result.GetString(), m_message);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class AssertionException : public Exception
{
public:
	AssertionException(Result result = RESULT_CODE_FAILURE, StringView message = "", std::source_location location = std::source_location::current());

private:
	static String CreateExceptionMessage(Result result, std::source_location const& location, StringView message = "");
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ApiException : public AssertionException
{
public:
	using AssertionException::AssertionException;
};

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_EXCEPTION_H
