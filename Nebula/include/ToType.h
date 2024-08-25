#ifndef NEBULA_TO_TYPE_H
#define NEBULA_TO_TYPE_H

#include "NebulaString.h"
#include "Result.h"

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
TTo ToTypeImpl(Nebula::StringView const from)
{
	assert('-' != from.front()); // Encoded value should never be signed.

	Nebula::Result result;

	TTo unsignedInteger = 0;

	for (size_t offset = 0; offset < from.size(); ++offset)
	{
		TTo thisDigit;
		result = Nebula::ToType<TTo>(from[offset], thisDigit);
		if (Nebula::RESULT_CODE_SUCCESS != result)
			break;

		// In case of overflow, return maximum possible value.
		if (((std::numeric_limits<TTo>::max() / 10) < unsignedInteger) ||
			((std::numeric_limits<TTo>::max() / 10) == unsignedInteger) && (0 != thisDigit))
		{
			unsignedInteger = std::numeric_limits<TTo>::max();
			break;
		}

		unsignedInteger *= 10;
		unsignedInteger += thisDigit;
	}

	return unsignedInteger;
}

} // detail ------------------------------------------------------------------------------------------------------------------------

namespace Nebula
{

template<typename TTo> requires IsUInt<TTo>
inline TTo ToType(StringView const from)
{
	return ToTypeImpl<TTo>(from);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TTo> requires IsSInt<TTo>
inline TTo ToType(StringView const from)
{
	bool const isNegative = ('-' == from.front());

	TTo integer = ToTypeImpl<TTo>(isNegative ? from.substr(1) : from);

	if (isNegative)
		integer *= static_cast<TTo>(-1);

	return integer;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TTo>
inline TTo ToType(String const& from)
{
	// Use ToType instead of ToTypeImpl to re-use concepts.
	return ToType<TTo>(MakeStringView(from));
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

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TO_TYPE_H
