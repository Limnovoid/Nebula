#ifndef NEBULA_RESULT_H
#define NEBULA_RESULT_H

#include "NebulaString.h"

// --------------------------------------------------------------------------------------------------------------------------------

#define RESULT_CODE_LIST(d)																	\
	d(RESULT_CODE_SUCCESS, "Success")														\
	d(RESULT_CODE_FAILURE, "Failure")														\
	d(RESULT_CODE_INVALID_PARAMETER, "Invalid Parameter")									\
	d(RESULT_CODE_MISSING_DEFINITION, "Missing Definition")									\
	d(RESULT_CODE_OUT_OF_RANGE, "Out of Range")												\
	d(RESULT_CODE_OVERFLOW, "Overflow")														\
	d(RESULT_CODE_UNDERFLOW, "Underflow")													\
	d(RESULT_CODE_UNCHANGED, "Unchanged")

#define RESULT_CODE_GET_ENUMS(resultCode, resultCodeString) resultCode,
#define RESULT_CODE_GET_SWITCH_CASES(resultCode, resultCodeString) case resultCode: return resultCodeString;
#define RESULT_CODE_STRINGS(resultCode, resultCodeString) static char const cstr_##resultCode[] = resultCodeString;

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

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
	Result();
	Result(ResultCode resultCode);

	ResultCode GetCode() const;

	char const* GetString() const;

	bool operator==(ResultCode const rhs) const;

private:
	ResultCode		m_resultCode;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline ResultCode Result::GetCode() const
{
	return m_resultCode;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline char const* Result::GetString() const
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

inline bool Result::operator==(ResultCode const rhs) const
{
	return m_resultCode == rhs;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline bool operator==(ResultCode const resultCode, Result const result)
{
	return (result == resultCode);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool operator==(Result const lhs, Result const rhs)
{
	return (lhs.GetCode() == rhs.GetCode());
}

// --------------------------------------------------------------------------------------------------------------------------------

inline String ToString(Result const& result)
{
	return String(result.GetString());
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#undef RESULT_CODE_LIST
#undef RESULT_CODE_GET_ENUMS
#undef RESULT_CODE_GET_SWITCH_CASES
#undef RESULT_CODE_STRINGS

#endif//NEBULA_RESULT_H
