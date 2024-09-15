#ifndef NEBULA_TO_TYPE_H
#define NEBULA_TO_TYPE_H

#include "NebulaString.h"
#include "Result.h"
#include "IUnitTest.h"

namespace Nebula
{

template<typename TTo> requires IsInt<TTo>
inline Result ToType(char const from, TTo & to)
{
	to = static_cast<TTo>(from) - static_cast<TTo>('0');

	return (to < 10) ? RESULT_CODE_SUCCESS : RESULT_CODE_INVALID_PARAMETER;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

namespace // detail
{

template<typename TTo> requires Nebula::IsInt<TTo>
Nebula::Result ToTypeImpl(Nebula::StringView const from, TTo & to)
{
	assert('-' != from.front()); // Encoded value should never be signed.

	Nebula::Result result;

	to = 0;

	for (size_t offset = 0; offset < from.size(); ++offset)
	{
		TTo thisDigit;
		result = Nebula::ToType<TTo>(from[offset], thisDigit);
		if (Nebula::RESULT_CODE_SUCCESS != result)
			break;

		// In case of overflow, return maximum possible value.
		if (((std::numeric_limits<TTo>::max() / 10) < to) ||
			((std::numeric_limits<TTo>::max() / 10) == to) && (0 != thisDigit))
		{
			to = std::numeric_limits<TTo>::max();
			result = Nebula::RESULT_CODE_OVERFLOW;
			break;
		}

		to *= 10;
		to += thisDigit;
	}

	return result;
}

} // detail ------------------------------------------------------------------------------------------------------------------------

namespace Nebula
{

template<typename TTo> requires IsUInt<TTo>
inline Result ToType(StringView from, TTo & to)
{
	return ToTypeImpl<TTo>(from, to);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TTo> requires IsSInt<TTo>
inline Result ToType(StringView from, TTo & to)
{
	bool const isNegative = ('-' == from.front());

	Result result = ToTypeImpl<TTo>(isNegative ? from.substr(1) : from, to);

	if (isNegative)
		to *= static_cast<TTo>(-1);

	return result;
}

// Conversion checks --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline bool IsNumeric(char const valueChar)
{
	return ((static_cast<uint8_t>(valueChar) - static_cast<uint8_t>('0')) < 10);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline bool IsNumeric(StringView const valueStringView)
{
	for (size_t i = ((valueStringView[0] == '-') ? 1 : 0); i < valueStringView.size(); ++i)
		if (!IsNumeric(valueStringView[i]))
			return false;

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

template<typename TTo> requires IsInt<TTo>
class UnitTestCharToType : public IUnitTest<TTo, char>
{
public:
	using IUnitTest = IUnitTest<TTo, char>;
	using TParameters = char;
	using TReturn = TTo;

	struct GetParameters
	{
		TParameters operator()(size_t index)
		{
			assert(index < 10);
			return '0' + static_cast<char>(index);
		}
	};

	struct GetExpected
	{
		TReturn operator()(size_t index)
		{
			assert(index < 10);
			return static_cast<TReturn>(index);
		}
	};

	UnitTestCharToType(StringView title) : IUnitTest(title) {}

	virtual ~UnitTestCharToType() {}

	virtual Result Invoke(TParameters const& parameters, TReturn & returned) override
	{
		return ToType<TTo>(parameters, returned);
	}
};

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TO_TYPE_H
