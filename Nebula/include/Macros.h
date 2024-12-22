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

#define IF_RESULT_CODE(name, op, expression) if (RESULT_CODE_##name op (expression))
#define IF_OR_RESULT_CODE(name1, name2, op, expression) if ((RESULT_CODE_##name1 op (expression)) || (RESULT_CODE_##name2 op (expression)))

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_MACROS_H
