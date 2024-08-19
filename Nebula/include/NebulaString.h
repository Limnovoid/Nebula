#ifndef NEBULA_STRING_H
#define NEBULA_STRING_H

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

using String = std::string;
using StringView = std::string_view;

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView MakeStringView(String const& string)
{
	return StringView(string.data(), string.length());
}

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView MakeStringView(char const& character)
{
	return StringView(&character, 1);
}

// Common String operations -------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

/// <returns>True if the character is printable whitespace; false otherwise.</returns>
bool IsWhitespace(char const c);

// --------------------------------------------------------------------------------------------------------------------------------

/// <summary> Get a view of the first word - unbroken sequence of non-whitespace characters - in the string. </summary>
StringView GetFirstWord(StringView string);

// ToString for basic types -------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

String ToString(bool const value);

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T> requires IsUInt<T>
String ToString(T value)
{
	static constexpr size_t maxDigits = Maths::NumDigits(std::numeric_limits<T>::max());
	static char stringBuffer[maxDigits + 1]{ '\0' };

	//size_t startOffset = maxDigits;
	//do
	//{
	//	assert(0 != startOffset); // Sanity check for underflow.
	//	--startOffset; // Decrement here so a value with maximum number of digits does not cause underflow.

	//	stringBuffer[startOffset] = static_cast<char>((value % 10) + static_cast<T>('0'));

	//	value /= 10;
	//}
	//while (0 != value);
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

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_STRING_H
