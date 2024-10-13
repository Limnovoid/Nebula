#ifndef NEBULA_EXCEPTION_H
#define NEBULA_EXCEPTION_H

#include "Result.h"
#include "Format.h"
#include "ITestProgram.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class Exception : std::exception
{
public:
	Exception(ResultCode resultCode, StringView message);

	ResultCode GetResultCode() const;
	StringView GetMessage() const;

	String ToString() const;

private:
	ResultCode	m_resultCode;
	String		m_message;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline ResultCode Exception::GetResultCode() const
{
	return m_resultCode;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView Exception::GetMessage() const
{
	return m_message;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline String Exception::ToString() const
{
	return Fmt::Format("Exception: {} - {}", Nebula::ToString(m_resultCode).c_str(), m_message.c_str());
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class AssertionException : public Exception
{
public:
	AssertionException(ResultCode resultCode = RESULT_CODE_FAILURE, StringView message = "", std::source_location location = std::source_location::current());

private:
	static String CreateExceptionMessage(std::source_location const& location, StringView message = "");
};

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_EXCEPTION_H
