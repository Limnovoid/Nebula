#ifndef NEBULA_TO_TYPE_H
#define NEBULA_TO_TYPE_H

#include "NebulaString.h"

namespace Nebula
{

template<typename TTo>
inline TTo ToType(char const from)
{
	uint8_t const digit = static_cast<uint8_t>(from) - static_cast<uint8_t>('0');

	assert(digit < 10);

	return digit;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

namespace // detail
{

template<typename TTo> requires Nebula::IsInt<TTo>
TTo ToTypeImpl(Nebula::StringView const from)
{
	assert('-' != from.front()); // Encoded value should never be signed.

	TTo unsignedInteger = 0;

	for (size_t offset = 0; offset < from.size(); ++offset)
	{
		TTo const thisDigit = Nebula::ToType<TTo>(from[offset]);

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

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TO_TYPE_H
