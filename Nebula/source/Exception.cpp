#include "Exception.h"

#include "Format.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

Exception::Exception(ResultCode resultCode, StringView message) :
	m_resultCode(),
	m_message(message)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

AssertionException::AssertionException(ResultCode resultCode, StringView message, std::source_location location) :
	Exception(resultCode, CreateExceptionMessage(location, message))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

String AssertionException::CreateExceptionMessage(std::source_location const& location, StringView message)
{
	String exceptionMessage = Fmt::Format("Assertion failed in {} ({}, line {})", location.function_name(), location.file_name(), location.line());

	if (!message.empty())
		(exceptionMessage += " - ") += message;

	return exceptionMessage;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
