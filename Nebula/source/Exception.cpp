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
	Exception(result, CreateExceptionMessage(result, location, message))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

String AssertionException::CreateExceptionMessage(Result result, std::source_location const& location, StringView message)
{
	String exceptionMessage = Fmt::Format("Assertion failed in {}, code {}", location.function_name(), result);

	if (!message.empty())
		exceptionMessage += Fmt::Format(" - {}", message);

	exceptionMessage += Fmt::Format(" ({}, line {})", location.file_name(), location.line());

	return exceptionMessage;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
