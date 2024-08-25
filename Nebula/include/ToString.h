#ifndef NEBULA_TO_STRING_H
#define NEBULA_TO_STRING_H

#include "NebulaString.h"
#include "Maths.h"

namespace // detail
{

template<typename T> requires Nebula::IsInt<T>
inline size_t ToStringImpl(T value, char * stringBuffer, size_t startOffset)
{
	do
	{
		assert(0 != startOffset); // Sanity check for underflow.
		--startOffset; // Decrement here so a value with maximum number of digits does not cause underflow.

		stringBuffer[startOffset] = static_cast<char>((value % 10) + static_cast<T>('0'));

		value /= 10;
	}
	while (0 != value);

	return startOffset;
}

} // detail ------------------------------------------------------------------------------------------------------------------------

namespace Nebula
{

// ToString for basic types -------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

String ToString(bool const value);

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T> requires IsUInt<T>
String ToString(T value)
{
	static constexpr size_t maxDigits = Maths::NumDigits(std::numeric_limits<T>::max());
	static char stringBuffer[maxDigits + 1]{ '\0' };

	size_t startOffset = ToStringImpl<T>(value, stringBuffer, maxDigits);

	return String(static_cast<char const*>(stringBuffer) + startOffset);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T> requires IsSInt<T>
String ToString(T value)
{
	static constexpr size_t maxDigits = Maths::NumDigits(std::numeric_limits<T>::max());
	static char stringBuffer[maxDigits + 2]{ '\0' };

	size_t startOffset = ToStringImpl<T>(Maths::Abs(value), stringBuffer, maxDigits);

	if (value < 0)
		stringBuffer[--startOffset] = '-';

	return String(static_cast<char const*>(stringBuffer) + startOffset);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline char ToUpper(char c)
{
	if (('a' <= c) && (c <= 'z'))
		c -= ('a' - 'A');

	return c;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline char ToLower(char c)
{
	if (('A' <= c) && (c <= 'Z'))
		c += ('a' - 'A');

	return c;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TO_STRING_H
