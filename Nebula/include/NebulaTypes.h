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
concept CSigned = std::is_signed_v<T>;

template<typename T>
concept CUnsigned = std::is_unsigned_v<T>;

template<typename T>
concept CInt = std::is_integral_v<T> && requires
{
	std::numeric_limits<T>::max();
	std::numeric_limits<T>::min();
};

template<typename T>
concept CUInt = CInt<T> && !CSigned<T>;

template<typename T>
concept CSInt = CInt<T> && CSigned<T>;

template<typename T>
concept CFloatingPoint = std::is_floating_point_v<T>;

template<typename T, typename... TArgs>
concept CInvocable = std::is_invocable_v<T, TArgs...>;

template<typename T>
concept CWriteable = requires (std::ostream & outputStream, T const& value)
{
	{ outputStream << value };
};

template<typename T>
concept CReadable = requires (std::istream & inputStream, T const& value)
{
	{ inputStream >> value };
};

template<typename T>
concept CReadWriteable = CWriteable<T> && CReadable<T>;

template<typename T>
concept CStringType = requires (T t)
{
	{ String(t) };
};

template<typename T>
concept CFormattable = requires (std::formatter<T> formatter, std::format_parse_context & parseCtx,
	std::format_context & formatCtx, T const& t)
{
	{ formatter.parse(parseCtx) };
	{ formatter.format(t, formatCtx) };
};

template<typename T, typename TOperand>
concept CCompareFunctor = std::is_invocable_r_v<bool, T, TOperand, TOperand>;

template<typename T>
concept CFundamental = std::is_fundamental_v<T>;

template<typename T>
concept CArithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept CGettableArithmetic = requires (T const& value)
{
	CArithmetic<typename T::ArithmeticType>;
	{ value.Get() } -> CArithmetic;
};

template<typename T, typename TBase>
concept CDerivesFrom = std::is_base_of_v<TBase, T>;

template<typename T>
concept CVoid = std::is_void_v<T>;

// Smart pointers -----------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename... TArgs>
inline SharedPtr<T> MakeShared(TArgs &&... args)
{
	return std::make_shared<T>(std::forward<TArgs>(args)...);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename... TArgs>
inline UniquePtr<T> MakeUnique(TArgs &&... args)
{
	return std::make_unique<T>(std::forward<TArgs>(args)...);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename U>
inline SharedPtr<T> StaticPtrCast(SharedPtr<U> pT)
{
	return std::static_pointer_cast<T, U>(pT);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename U>
inline SharedPtr<T> DynamicPtrCast(SharedPtr<U> pT)
{
	return std::dynamic_pointer_cast<T, U>(pT);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline bool operator==(WeakPtr<T> lhs, WeakPtr<T> rhs)
{
	assert(!lhs.expired());
	assert(!rhs.expired());

	return lhs.lock() == rhs.lock();
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TYPES_H
