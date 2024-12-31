#ifndef NEBULA_RESULT_H
#define NEBULA_RESULT_H

#include "NebulaString.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

#define RESULT_CODE_LIST(d)																	\
	d(RESULT_CODE_SUCCESS, "Success")														\
	d(RESULT_CODE_FAILURE, "Failure")														\
	d(RESULT_CODE_INVALID_PARAMETER, "Invalid Parameter")									\
	d(RESULT_CODE_MISSING_DEFINITION, "Missing Definition")									\
	d(RESULT_CODE_OUT_OF_RANGE, "Out of Range")												\
	d(RESULT_CODE_OVERFLOW, "Overflow")														\
	d(RESULT_CODE_UNDERFLOW, "Underflow")													\
	d(RESULT_CODE_UNCHANGED, "Unchanged")													\
	d(RESULT_CODE_ALREADY_INITIALIZED, "Already Initialized")								\
	d(RESULT_CODE_NOT_INITIALIZED, "Not Initialized")										\
	d(RESULT_CODE_NOT_OPEN, "Not Open")														\
	d(RESULT_CODE_ALREADY_OPEN, "Already Open")												\
	d(RESULT_CODE_NOT_IMPLEMENTED, "Not Implemented")										\
	d(RESULT_CODE_EMPTY, "Empty")


#define RESULT_CODE_GET_ENUMS(resultCode, resultCodeString) resultCode,
#define RESULT_CODE_GET_SWITCH_CASES(resultCode, resultCodeString) case resultCode: return resultCodeString;
#define RESULT_CODE_STRINGS(resultCode, resultCodeString) static char const cstr_##resultCode[] = resultCodeString;

// --------------------------------------------------------------------------------------------------------------------------------

#define RETURN_RESULT_IF(resultCode, comparisonOperator, expression) { if (Result const result = expression; result comparisonOperator resultCode) return result; }

#ifdef  _DEBUG
#define DEBUG_RETURN_RESULT_IF(resultCode, comparisonOperator, expression) RETURN_RESULT_IF(resultCode, comparisonOperator, expression)
#else //_DEBUG
#define DEBUG_RETURN_RESULT_IF(resultCode, comparisonOperator, expression) { expression }
#endif//_DEBUG

// --------------------------------------------------------------------------------------------------------------------------------

enum ResultCode : uint8_t
{
	RESULT_CODE_LIST(RESULT_CODE_GET_ENUMS)
	RESULT_CODE_COUNT
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Result
{
public:
	Result() = delete;
	constexpr Result(Result const& rhs);
	constexpr Result(ResultCode resultCode);

	constexpr ResultCode GetCode() const;

	constexpr char const* GetString() const;

	constexpr bool operator==(ResultCode const rhs) const;

private:
	ResultCode	m_resultCode;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr Result::Result(Result const& rhs) :
	m_resultCode(rhs.m_resultCode)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr Result::Result(ResultCode resultCode) :
	m_resultCode(resultCode)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr ResultCode Result::GetCode() const
{
	return m_resultCode;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr char const* Result::GetString() const
{
	switch (m_resultCode)
	{
		RESULT_CODE_LIST(RESULT_CODE_GET_SWITCH_CASES)

	default:
		assert(false); // We shouldn't be here!
	}
	return "";
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr bool Result::operator==(ResultCode const rhs) const
{
	return m_resultCode == rhs;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr bool operator==(ResultCode const resultCode, Result const result)
{
	return (result == resultCode);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr bool operator==(Result const lhs, Result const rhs)
{
	return (lhs.GetCode() == rhs.GetCode());
}

// --------------------------------------------------------------------------------------------------------------------------------

inline String ToString(Result const& result)
{
	return String(result.GetString());
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Nebula::Result> : std::formatter<uint8_t>
{
	auto format(Nebula::Result const& result, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", result.GetString());
	}
};

// --------------------------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Nebula::ResultCode> : std::formatter<uint8_t>
{
	auto format(Nebula::ResultCode const resultCode, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", Nebula::Result(resultCode).GetString());
	}
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

#undef RESULT_CODE_LIST
#undef RESULT_CODE_GET_ENUMS
#undef RESULT_CODE_GET_SWITCH_CASES
#undef RESULT_CODE_STRINGS

#endif//NEBULA_RESULT_H
