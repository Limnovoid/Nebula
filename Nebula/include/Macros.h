#ifndef NEBULA_MACROS_H
#define NEBULA_MACROS_H

#include "Exception.h"

namespace
{

void ThrowOnAssertFailed(Nebula::StringView functionName)
{
	using namespace Nebula;

	String exceptionMessage = functionName;

	throw Exception(RESULT_CODE_FAILURE, exceptionMessage);
}

} // namespace --------------------------------------------------------------------------------------------------------------------

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

#ifdef  _DEBUG
#define ASSERT(expression) assert(expression)
#elif
#define ASSERT(expression)
#endif//_DEBUG

#if defined(_DEBUG) && defined(EXCEPTION_ASSERT_BREAK_ON_FAIL)
#define ASSERT_THROW(expression, resultCode, message) { assert(expression); if (!expression) { throw AssertionException(std::source_location::current(), resultCode, message); } }
#else
#define ASSERT_THROW(expression, resultCode, message) { if (!(expression)) { throw AssertionException(resultCode, message, std::source_location::current()); } }
#endif//_DEBUG

#ifdef _DEBUG
#define DEBUG_ONLY(expression) expression
#else
#define DEBUG_ONLY(expression)
#endif

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_MACROS_H
