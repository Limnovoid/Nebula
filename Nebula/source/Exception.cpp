#include "Exception.h"

#include "Format.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

Exception::Exception(Result result, StringView message) :
	m_result(result),
	m_message(message)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

AssertionException::AssertionException(Result result, StringView message, std::source_location location) :
	Exception(result, CreateExceptionMessage(location, message))
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
