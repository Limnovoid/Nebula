#ifndef NEBULA_TO_TYPE_H
#define NEBULA_TO_TYPE_H

namespace Nebula
{

template<typename T>
concept IsInt = std::is_integral_v<T>;

template<typename T>
concept IsUInt = IsInt<T> && std::is_unsigned_v<T>;

template<typename T>
concept IsSInt = IsInt<T> && std::is_signed_v<T>;

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TTo> requires IsUInt<TTo>
inline TTo ToType(StringView const from)
{
	return TTo();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TTo> requires IsSInt<TTo>
inline TTo ToType(StringView const from)
{
	return TTo();
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename TTo>
inline TTo ToType(char const from)
{
	return ToType<TTo>(MakeStringView(from));
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TO_TYPE_H
