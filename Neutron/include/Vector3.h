#ifndef NEUTRON_VECTOR_3_H
#define NEUTRON_VECTOR_3_H

#include "Maths.h"
#include "ITestScript.h"
#include "ConstString.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
class TVector3
{
public:
	constexpr TVector3();
	constexpr TVector3(T v);
	constexpr TVector3(T x, T y, T z);
	constexpr TVector3(TVector3 const& rhs);
	template<typename U> constexpr TVector3(TVector3<U> const& rhs);

	/// <returns> (0, 0, 0) </returns>
	static constexpr TVector3 Zero();

	/// <returns> (1, 0, 0) </returns>
	static constexpr TVector3 X1();

	/// <returns> (0, 1, 0) </returns>
	static constexpr TVector3 Y1();

	/// <returns> (0, 0, 1) </returns>
	static constexpr TVector3 Z1();

	/// <summary> Compute the vector cross product. Optimised for precision when operating on vector components with very different magnitudes. </summary>
	/// <returns> The cross product = lhs x rhs. </returns>
	static TVector3 PreciseCross(TVector3 const& lhs, TVector3 const& rhs);

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
	TVector3 Normalized() const;

	/// <returns> The dot product of this vector and another. </returns>
	T Dot(TVector3 const& rhs) const;

	/// <returns> The cross product of this vector (the left-hand side) and another (the right-hand side) = lhs x rhs. </returns>
	TVector3 Cross(TVector3 const& rhs) const;

	/// <summary> Compute the vector cross product. Optimised for precision when operating on vector components with very different magnitudes. </summary>
	/// <returns> The cross product = lhs x rhs. </returns>
	TVector3 PreciseCross(TVector3 const& rhs);

	/// <summary> Normalize this vector. </summary>
	/// <returns> A reference to this (normalized) vector. </returns>
	TVector3 & Normalize();

	bool operator==(TVector3 const& rhs) const;
	bool operator!=(TVector3 const& rhs) const;
	TVector3 operator+(TVector3 const& rhs) const;
	TVector3 operator-(TVector3 const& rhs) const;
	TVector3 operator*(const T scalar) const;
	TVector3 operator/(const T scalar) const;
	TVector3 & operator=(TVector3 const& rhs);
	TVector3 & operator+=(TVector3 const& rhs);
	TVector3 & operator-=(TVector3 const& rhs);
	TVector3 & operator*=(const T scalar);
	TVector3 & operator/=(const T scalar);

	inline static constexpr ConstString FORMAT_STRING = "{} {} {}";

private:
	T	m_x, m_y, m_z;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline constexpr TVector3<T>::TVector3() :
	m_x(0),
	m_y(0),
	m_z(0)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline constexpr TVector3<T>::TVector3(T v) :
	m_x(v),
	m_y(v),
	m_z(v)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline constexpr TVector3<T>::TVector3(T x, T y, T z) :
	m_x(x),
	m_y(y),
	m_z(z)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline constexpr TVector3<T>::TVector3(TVector3 const& rhs) :
	m_x(rhs.m_x),
	m_y(rhs.m_y),
	m_z(rhs.m_z)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr TVector3<T>::TVector3(TVector3<U> const& rhs) :
	m_x(static_cast<T>(rhs.m_x)),
	m_y(static_cast<T>(rhs.m_y)),
	m_z(static_cast<T>(rhs.m_z))
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline constexpr TVector3<T> TVector3<T>:: Zero()
{
	return { 0, 0, 0 };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline constexpr TVector3<T> TVector3<T>:: X1()
{
	return { 1, 0, 0 };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline constexpr TVector3<T> TVector3<T>:: Y1()
{
	return { 0, 1, 0 };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline constexpr TVector3<T> TVector3<T>:: Z1()
{
	return { 0, 0, 1 };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T> TVector3<T>::PreciseCross(TVector3 const& lhs, TVector3 const& rhs)
{
	T lhsMagnitude = Maths::Sqrt<T>(lhs.SqareMagnitude());
	T rhsMagnitude = Maths::Sqrt<T>(rhs.SqareMagnitude());

	TVector3 lhsNormalized = lhs / lhsMagnitude;
	TVector3 rhsNormalized = rhs / rhsMagnitude;

	TVector3 crossDirection = lhsNormalized.Cross(rhsNormalized);

	T magnitudeProduct = lhsMagnitude * rhsMagnitude;

	T cosAngle = ((lhs.m_x * rhs.m_x) + (lhs.m_y * rhs.m_y) + (lhs.m_z * rhs.m_z)) / magnitudeProduct;

	T sinAngle = Maths::Sqrt<T>(static_cast<T>(1) - (cosAngle * cosAngle)); // Trig. ident. 1 = sin^2(a) + cos^2(a)

	T crossMagnitude = lhsMagnitude * rhsMagnitude * sinAngle;

	return crossDirection * crossMagnitude;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T TVector3<T>::X() const
{
	return m_x;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T TVector3<T>::Y() const
{
	return m_y;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T TVector3<T>::Z() const
{
	return m_z;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline bool TVector3<T>::IsZero() const
{
	return (m_x == static_cast<T>(0)) && (m_y == static_cast<T>(0)) && (m_z == static_cast<T>(0));
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T TVector3<T>::SqareMagnitude() const
{
	return (m_x * m_x) + (m_y * m_y) + (m_z * m_z);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T> TVector3<T>::Normalized() const
{
	T sqareMagnitude = SqareMagnitude();

	if (sqareMagnitude == 0)
		return *this;

	return *this / Maths::Sqrt<T>(sqareMagnitude);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T TVector3<T>::Dot(TVector3<T> const& rhs) const
{
	return (m_x * rhs.m_x) + (m_y * rhs.m_y) + (m_z * rhs.m_z);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T> TVector3<T>::Cross(TVector3<T> const& rhs) const
{
	return {
		(m_y * rhs.m_z) - (m_z * rhs.m_y),
		(m_z * rhs.m_x) - (m_x * rhs.m_z),
		(m_x * rhs.m_y) - (m_y * rhs.m_x)
	};
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T> TVector3<T>::PreciseCross(TVector3 const& rhs)
{
	return PreciseCross(*this, rhs);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T>& TVector3<T>::Normalize()
{
	T sqrMag = this->SqareMagnitude();

	if (sqrMag == 0)
		return *this;

	return (*this) /= sqrt(sqrMag);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline bool TVector3<T>::operator==(TVector3 const& rhs) const
{
	return (m_x == rhs.m_x) && (m_y == rhs.m_y) && (m_z == rhs.m_z);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline bool TVector3<T>::operator!=(TVector3 const& rhs) const
{
	return (m_x != rhs.m_x) || (m_y != rhs.m_y) || (m_z != rhs.m_z);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T> TVector3<T>::operator+(TVector3 const& rhs) const
{
	return { m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T> TVector3<T>::operator-(TVector3 const& rhs) const
{
	return { m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T> TVector3<T>::operator*(const T scalar) const
{
	return { scalar * m_x, scalar * m_y, scalar * m_z };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T> TVector3<T>::operator/(const T scalar) const
{
	return { m_x / scalar, m_y / scalar, m_z / scalar };
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T> & TVector3<T>::operator=(TVector3 const& rhs)
{
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	m_z = rhs.m_z;

	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T>& TVector3<T>::operator+=(TVector3 const& rhs)
{
	m_x += rhs.m_x;
	m_y += rhs.m_y;
	m_z += rhs.m_z;

	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T>& TVector3<T>::operator-=(TVector3 const& rhs)
{
	m_x -= rhs.m_x;
	m_y -= rhs.m_y;
	m_z -= rhs.m_z;

	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T>& TVector3<T>::operator*=(const T scalar)
{
	m_x *= scalar;
	m_y *= scalar;
	m_z *= scalar;

	return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline TVector3<T>& TVector3<T>::operator/=(const T scalar)
{
	m_x /= scalar;
	m_y /= scalar;
	m_z /= scalar;

	return *this;
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
