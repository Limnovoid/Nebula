#ifndef NEBULA_TYPES_H
#define NEBULA_TYPES_H

namespace Nebula
{

// Type constraints ------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
concept IsSigned = std::is_signed_v<T>;

template<typename T>
concept IsUnsigned = std::is_unsigned_v<T>;

template<typename T>
concept IsInt = std::is_integral_v<T> && requires
{
	std::numeric_limits<T>::max();
	std::numeric_limits<T>::min();
};

template<typename T>
concept IsUInt = IsInt<T> && !IsSigned<T>;

template<typename T>
concept IsSInt = IsInt<T> && IsSigned<T>;

template<typename T>
concept IsFloatingPoint = std::is_floating_point_v<T>;

// Smart pointers -----------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
using SharedPtr = std::shared_ptr<T>;

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline SharedPtr<T> MakeShared(T * ptr)
{
	return std::make_shared<T>(ptr);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename... TArgs>
inline SharedPtr<T> MakeShared(TArgs... args)
{
	return std::make_shared<T>(std::forward<TArgs>(args)...);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TYPES_H
