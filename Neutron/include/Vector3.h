#ifndef NEUTRON_VECTOR_3_H
#define NEUTRON_VECTOR_3_H

#include "Maths.h"
#include "ITestScript.h"
#include "ConstString.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP = void>
class TVector3
{
	friend class TVector3;

public:
	static constexpr bool IS_USING_CRTP = !std::is_void_v<CRTP>;

	using TReturnVector3 = std::conditional<IS_USING_CRTP, CRTP, TVector3>::type;
	using TPass = std::conditional<CArithmetic<T>, T, T&>::type;

	/// <summary> Compute the vector cross product. Optimised for precision when operating on vector components with very different magnitudes. </summary>
	/// <param name="magnitude"> Storage for the magnitude of the computed cross product. </param>
	/// <param name="direction"> Storage for the direction of the computed cross product. </param>
	/// <returns> The cross product = lhs x rhs. </returns>
	static void PreciseCross(TVector3 const& lhs, TVector3 const& rhs, T & magnitude, TVector3 & direction);

	/// <param name="tolerance"> The vectors are approximately parallel if their dot product is greater than this tolerance. </param>
	/// <returns> Whether the vectors are approximately parallel. </returns>
	static bool AreApproxParallel(TVector3 const& lhs, TVector3 const& rhs, T tolerance = Maths::Epsilon<T>());

	/// <summary>
	/// Compute the angle (radians) between two unit vectors.
	/// Handles floating point error which can result in a dot product being greater than 1 for approximately-parallel unit vectors.
	/// </summary>
	static T AngleBetweenUnitVectors(TVector3 const& lhs, TVector3 const& rhs);

	//static constexpr TVector3 Zero()		{ return TVector3(T(0)); }
	//static constexpr TVector3 X1()		{ return TVector3(T(1), T(0), T(0)); }
	//static constexpr TVector3 Y1()		{ return TVector3(T(0), T(1), T(0)); }
	//static constexpr TVector3 Z1()		{ return TVector3(T(0), T(0), T(1)); }

	inline static constexpr ConstString		FORMAT_STRING = "{} {} {}";

	static const TReturnVector3				ZERO;
	static const TReturnVector3				X1;
	static const TReturnVector3				Y1;
	static const TReturnVector3				Z1;

public:
	constexpr TVector3();
	constexpr TVector3(TVector3 const& rhs);
	constexpr TVector3(TReturnVector3 const& rhs) requires IS_USING_CRTP;
	constexpr TVector3(const T v);
	constexpr TVector3(const T x, const T y, const T z);
	template<typename U> constexpr TVector3(TVector3<U> const& rhs);

	/// <returns> The X-component. </returns>
	T X() const;

	/// <returns> The Y-component. </returns>
	T Y() const;

	/// <returns> The Z-component. </returns>
	T Z() const;

	/// <returns> Whether this vector is the zero vector (all components equal zero). </returns>
	bool IsZero() const;

	/// <returns> The square magnitude of this vector. </returns>
	T SqareMagnitude() const;

	/// <returns> A normalized copy of this vector. </returns>
	TReturnVector3 Normalized() const;

	/// <returns> The dot product of this vector and another. </returns>
	T Dot(TVector3 const& rhs) const;

	/// <returns> The cross product of this vector (the left-hand side) and another (the right-hand side) = lhs x rhs. </returns>
	TReturnVector3 Cross(TVector3 const& rhs) const;

	/// <summary> Compute the vector cross product = this x rhs. Optimised for precision when operating on vector components with very different magnitudes. </summary>
	/// <param name="magnitude"> Storage for the magnitude of the computed cross product. </param>
	/// <param name="direction"> Storage for the direction of the computed cross product. </param>
	void PreciseCross(TVector3 const& rhs, T & magnitude, TVector3 & direction) const;

	/// <summary> Compute the vector cross product. Optimised for precision when operating on vector components with very different magnitudes. </summary>
	/// <returns> The cross product = lhs x rhs. </returns>
	TReturnVector3 PreciseCross(TVector3 const& rhs) const;

	/// <returns> Whether this and another vector are approximately parallel. </returns>
	bool IsApproxParallel(TVector3 const& other, T tolerance = std::numeric_limits<T>::epsilon()) const;

	/// <summary> Normalize this vector. </summary>
	/// <returns> A reference to this (normalized) vector. </returns>
	TReturnVector3 & Normalize();

	bool operator==(TVector3 const& rhs) const;
	bool operator!=(TVector3 const& rhs) const;
	TReturnVector3 operator+(TVector3 const& rhs) const;
	TReturnVector3 operator-(TVector3 const& rhs) const;
	TReturnVector3 operator*(const T scalar) const;
	TReturnVector3 operator/(const T scalar) const;
	TReturnVector3 & operator=(TVector3 const& rhs);
	TReturnVector3 & operator+=(TVector3 const& rhs);
	TReturnVector3 & operator-=(TVector3 const& rhs);
	TReturnVector3 & operator*=(const T scalar);
	TReturnVector3 & operator/=(const T scalar);

private:
	T									m_x, m_y, m_z;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP> const TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::ZERO		= { T(1) };
template<typename T, typename CRTP> const TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::X1		= { T(1), T(0), T(0) };
template<typename T, typename CRTP> const TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::Y1		= { T(0), T(1), T(0) };
template<typename T, typename CRTP> const TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::Z1		= { T(0), T(0), T(1) };

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline void TVector3<T, CRTP>::PreciseCross(TVector3 const& lhs, TVector3 const& rhs, T & magnitude, TVector3 & direction)
{
	T lhsMagnitude = Maths::Sqrt<T>(lhs.SqareMagnitude());
	T rhsMagnitude = Maths::Sqrt<T>(rhs.SqareMagnitude());

	TVector3 lhsNormalized = lhs / lhsMagnitude;
	TVector3 rhsNormalized = rhs / rhsMagnitude;

	direction = lhsNormalized.Cross(rhsNormalized).Normalize();

	T magnitudeProduct = lhsMagnitude * rhsMagnitude;

	T cosAngle = ((lhs.m_x * rhs.m_x) + (lhs.m_y * rhs.m_y) + (lhs.m_z * rhs.m_z)) / magnitudeProduct;

	T sinAngle = Maths::Sqrt<T>(static_cast<T>(1) - (cosAngle * cosAngle)); // Trig. ident. 1 = sin^2(a) + cos^2(a)

	magnitude = magnitudeProduct * sinAngle;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline bool TVector3<T, CRTP>::AreApproxParallel(TVector3 const& lhs, TVector3 const& rhs, T tolerance)
{
	return ((T(1) - tolerance) < lhs.Dot(rhs));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline T TVector3<T, CRTP>::AngleBetweenUnitVectors(TVector3 const& lhs, TVector3 const& rhs)
{
	assert(T(1) == lhs.SqareMagnitude());
	assert(T(1) == rhs.SqareMagnitude());

	return T(Maths::Acos(Maths::Clamp(lhs.Dot(rhs), T(-1), T(1)))); // Clamp in case of precision error.
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr TVector3<T, CRTP>::TVector3() :
	m_x(0),
	m_y(0),
	m_z(0)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr TVector3<T, CRTP>::TVector3(TVector3 const& rhs) :
	m_x(rhs.m_x),
	m_y(rhs.m_y),
	m_z(rhs.m_z)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr TVector3<T, CRTP>::TVector3(TReturnVector3 const& rhs) requires IS_USING_CRTP :
	m_x(rhs.m_x),
	m_y(rhs.m_y),
	m_z(rhs.m_z)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr TVector3<T, CRTP>::TVector3(const T v) :
	m_x(v),
	m_y(v),
	m_z(v)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline constexpr TVector3<T, CRTP>::TVector3(const T x, const T y, const T z) :
	m_x(x),
	m_y(y),
	m_z(z)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
template<typename U>
inline constexpr TVector3<T, CRTP>::TVector3(TVector3<U> const& rhs) :
	m_x(T(rhs.m_x)),
	m_y(T(rhs.m_y)),
	m_z(T(rhs.m_z))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline T TVector3<T, CRTP>::X() const
{
	return m_x;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline T TVector3<T, CRTP>::Y() const
{
	return m_y;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline T TVector3<T, CRTP>::Z() const
{
	return m_z;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline bool TVector3<T, CRTP>::IsZero() const
{
	return (m_x == static_cast<T>(0)) && (m_y == static_cast<T>(0)) && (m_z == static_cast<T>(0));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline T TVector3<T, CRTP>::SqareMagnitude() const
{
	return (m_x * m_x) + (m_y * m_y) + (m_z * m_z);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::Normalized() const
{
	const T sqareMagnitude = SqareMagnitude();

	if (T(0) == sqareMagnitude)
		return *static_cast<const TReturnVector3 *>(this);

	return *static_cast<const TReturnVector3 *>(this) / Maths::Sqrt<T>(sqareMagnitude);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline T TVector3<T, CRTP>::Dot(TVector3<T, CRTP> const& rhs) const
{
	return (m_x * rhs.m_x) + (m_y * rhs.m_y) + (m_z * rhs.m_z);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::Cross(TVector3<T, CRTP> const& rhs) const
{
	return {
		(m_y * rhs.m_z) - (m_z * rhs.m_y),
		(m_z * rhs.m_x) - (m_x * rhs.m_z),
		(m_x * rhs.m_y) - (m_y * rhs.m_x)
	};
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline void TVector3<T, CRTP>::PreciseCross(TVector3 const& rhs, T & magnitude, TVector3 & direction) const
{
	PreciseCross(*this, rhs, magnitude, direction);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::PreciseCross(TVector3 const& rhs) const
{
	// No need to construct temporary variables in every call - values are assigned by call to PreciseCross().
	static T magnitude = T(0);
	static TVector3 direction(T(0));

	PreciseCross(*this, rhs, magnitude, direction);

	return TReturnVector3(magnitude * direction);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
bool TVector3<T, CRTP>::IsApproxParallel(TVector3 const& other, T tolerance) const
{
	return TVector3::AreApproxParallel(*this, other, tolerance);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 & TVector3<T, CRTP>::Normalize()
{
	T sqrMag = this->SqareMagnitude();

	if (sqrMag == 0)
		return static_cast<TReturnVector3 &>(*this);

	return static_cast<TReturnVector3 &>((*this) /= Maths::Sqrt(sqrMag));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline bool TVector3<T, CRTP>::operator==(TVector3 const& rhs) const
{
	return (m_x == rhs.m_x) && (m_y == rhs.m_y) && (m_z == rhs.m_z);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline bool TVector3<T, CRTP>::operator!=(TVector3 const& rhs) const
{
	return (m_x != rhs.m_x) || (m_y != rhs.m_y) || (m_z != rhs.m_z);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::operator+(TVector3 const& rhs) const
{
	return { m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::operator-(TVector3 const& rhs) const
{
	return { m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::operator*(const T scalar) const
{
	return { scalar * m_x, scalar * m_y, scalar * m_z };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 TVector3<T, CRTP>::operator/(const T scalar) const
{
	return { m_x / scalar, m_y / scalar, m_z / scalar };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 & TVector3<T, CRTP>::operator=(TVector3 const& rhs)
{
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	m_z = rhs.m_z;

	return static_cast<TReturnVector3 &>(*this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 & TVector3<T, CRTP>::operator+=(TVector3 const& rhs)
{
	m_x += rhs.m_x;
	m_y += rhs.m_y;
	m_z += rhs.m_z;

	return static_cast<TReturnVector3 &>(*this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 & TVector3<T, CRTP>::operator-=(TVector3 const& rhs)
{
	m_x -= rhs.m_x;
	m_y -= rhs.m_y;
	m_z -= rhs.m_z;

	return static_cast<TReturnVector3 &>(*this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 & TVector3<T, CRTP>::operator*=(const T scalar)
{
	m_x *= scalar;
	m_y *= scalar;
	m_z *= scalar;

	return static_cast<TReturnVector3 &>(*this);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 & TVector3<T, CRTP>::operator/=(const T scalar)
{
	m_x /= scalar;
	m_y /= scalar;
	m_z /= scalar;

	return static_cast<TReturnVector3 &>(*this);
}

// --------------------------------------------------------------------------------------------------------------------------------

//template<typename T, typename CRTP>
//template<typename UReturnVector3> requires (!std::is_void_v<CRTP> && std::is_same_v<UReturnVector3, typename TVector3<T, CRTP>::TReturnVector3>)
//inline constexpr TVector3<T, CRTP>::TVector3(UReturnVector3 const& rhs) :
//	TVector3(static_cast<TVector3 const&>(rhs))
//{
//}

// --------------------------------------------------------------------------------------------------------------------------------
// Non-member Binary Operators
// --------------------------------------------------------------------------------------------------------------------------------


template<typename T, typename CRTP>
inline TVector3<T, CRTP>::TReturnVector3 operator*(const typename TVector3<T, CRTP>::TPass scalar, TVector3<T, CRTP> const& vector)
{
	return vector * scalar;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

using Vector3 = TVector3<float>;
using Vector3d = TVector3<double>;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Vector3TestScript : public ITestScript
{
public:
	Vector3TestScript();
	virtual ~Vector3TestScript();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Neutron::Vector3> : std::formatter<uint8_t>
{
	auto format(Neutron::Vector3 const& vector3, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), Neutron::Vector3::FORMAT_STRING.Get(), vector3.X(), vector3.Y(), vector3.Z());
	}
};

#endif//NEUTRON_VECTOR_3_H
