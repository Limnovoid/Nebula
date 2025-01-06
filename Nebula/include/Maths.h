#ifndef NEBULA_MATHS_H
#define NEBULA_MATHS_H

#include "NebulaTypes.h"

namespace // detail
{

inline constexpr int64_t PowImpl(int64_t base, int32_t exp, int64_t result)
{
	return (1 > exp) ? result : PowImpl(base * base, exp / 2, (((exp % 2) == 1) ? result * base : result));
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline constexpr size_t NumDigitsIntImpl(uint64_t value, uint64_t result = 1)
{
	return (value < 10) ? result : NumDigitsIntImpl(value / 10, result + 1);
}

} // detail ------------------------------------------------------------------------------------------------------------------------

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

namespace Maths
{

template<typename T>
inline constexpr T Abs(T signedValue)
{
	return (signedValue < 0) ? (signedValue * static_cast<T>(-1)) : signedValue;
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline constexpr int64_t Pow(int64_t base, int32_t exp)
{
	return PowImpl(base, exp, 1);
}

// ---------------------------------------------------------------------------------------------------------------------------------

inline constexpr bool IsPowerOf2(uint64_t n)
{
	return (0 == (n & (n - 1)));
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T> requires IsUInt<T>
inline constexpr size_t NumDigits(T value)
{
	return NumDigitsIntImpl(value, 1);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T> requires IsSInt<T>
inline constexpr size_t NumDigits(T value)
{
	return NumDigitsIntImpl(Abs(value), 1);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T> requires IsFloatingPoint<T>
inline constexpr size_t NumDigits(T value)
{
	/*T const absValue = Abs(value);

	uint64_t integerPart = static_cast<uint64_t>(absValue);

	size_t numIntegerDigits = NumDigits(integerPart);

	T decimalPart = absValue - integerPart;

	integerPart = 0;
	while (0 != decimalPart)
	{
		decimalPart *= static_cast<T>(10.0);

		integerPart *= 10;
		integerPart += static_cast<uint64_t>();
	}*/

	static_assert(false); // TODO !
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T> requires IsFloatingPoint<T>
inline T Ceiling(T value)
{
	return std::ceil(value);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T> requires IsFloatingPoint<T>
inline T Floor(T value)
{
	return std::floor(value);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T Sqrt(T value)
{
	static_assert(false);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<>
inline float Sqrt<>(float value)
{
	return sqrtf(value);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<>
inline double Sqrt<>(double value)
{
	return sqrt(value);
}

// ---------------------------------------------------------------------------------------------------------------------------------

template<>
inline long double Sqrt<>(long double value)
{
	return sqrtl(value);
}

} // namespace Maths --------------------------------------------------------------------------------------------------------------

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_MATHS_H
