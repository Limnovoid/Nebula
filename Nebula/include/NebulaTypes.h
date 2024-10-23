#ifndef NEBULA_TYPES_H
#define NEBULA_TYPES_H

#include "Stable.h"
#include "NebulaString.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

using byte_t = uint8_t;

// Type constraints & Concepts -------------------------------------------------------------------------------------------------
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

template<typename T, typename... TArgs>
concept IsInvocable = std::is_invocable_v<T, TArgs...>;

template<typename T>
concept IsWriteable = requires (std::ostream & outputStream, T const& value)
{
	{ outputStream << value };
};

template<typename T>
concept IsReadable = requires (std::istream & inputStream, T const& value)
{
	{ inputStream >> value };
};

template<typename T>
concept IsReadWriteable = requires (std::iostream & inputOutputStream, T const& value)
{
	{ inputOutputStream << value };
	{ inputOutputStream >> value };
};

template<typename T>
concept IsStringType = requires (T t)
{
	{ String(t) };
};

template<typename T>
concept IsVoid = std::is_void_v<T>;

// Smart pointers -----------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename... TArgs>
inline SharedPtr<T> MakeShared(TArgs... args)
{
	return std::make_shared<T>(std::forward<TArgs>(args)...);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename U>
inline SharedPtr<T> StaticPtrCast(SharedPtr<U> pT)
{
	return std::static_pointer_cast<T, U>(pT);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TYPES_H
