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

#ifdef _DEBUG
#define DEBUG_ONLY(expression) expression
#else
#define DEBUG_ONLY(expression)
#endif

#define RESULT_CODE(name) RESULT_CODE_##name

#define IF_RESULT_CODE(name, op, expression) if (RESULT_CODE(name) op (expression))
#define IF_OR_RESULT_CODE(name1, name2, op, expression) if ((RESULT_CODE_##name1 op (expression)) || (RESULT_CODE_##name2 op (expression)))

#ifdef _DEBUG
#define ASSERT_THROW(expression, resultCode, message) { if (!(expression)) { throw AssertionException(resultCode, message, std::source_location::current()); } }

#define ASSERT_THROW_RESULT(name, op, expression, message) { Result const result = (expression);\
	IF_RESULT_CODE(name, op, result) { throw AssertionException(result, message, std::source_location::current()); } }
#endif//_DEBUG

#ifdef _DEBUG
#define DEBUG_ONLY(expression) expression
#else
#define DEBUG_ONLY(expression)
#endif

#define API_ASSERT_THROW(expression, resultCode, message) { if (!(expression)) { throw ApiException(resultCode, message, std::source_location::current()); } }

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_MACROS_H
