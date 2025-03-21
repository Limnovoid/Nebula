#include "Orbit.h"

#include "ScalingSpace.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

void Orbit::Elements::Compute(float gravityParameter, Vector3 const& position, Vector3 const& velocity)
{
	// Angular momentum (H) = R x V.
	position.PreciseCross(velocity, m_angularMomentum, m_perifocalZ);

	Vector3 angularMomentumVector = m_angularMomentum * m_perifocalZ;

	API_ASSERT_THROW(0 < m_angularMomentum, RESULT_CODE_INVALID_PARAMETER,
		Fmt::Format("Angular momentum evaluated to zero from position ({}), velocity ({}).", position, velocity));

	m_parameter = m_angularMomentum * m_angularMomentum / gravityParameter; // Orbit parameter (p) = H^2 / g.
	m_velocityK = gravityParameter / m_angularMomentum;
	m_massK = gravityParameter * gravityParameter / powf(m_angularMomentum, 3.f);

	Vector3 positionDirection = position.Normalized();

	Vector3 eccentrictyVector = (velocity.PreciseCross(angularMomentumVector) / gravityParameter) - positionDirection;
	float eccentricitySquared = eccentrictyVector.SqareMagnitude();
	m_eccentricity = Maths::Sqrt(eccentricitySquared); // Eccentricity (e) = | ((V X H) / u) - (R / r) |.

	float eccentricityTerm; // Eccentricity term (e').
	if (m_eccentricity < kEccentricityEpsilon)
	{
		m_eccentricity = 0.f;
		m_type = Type::Circle;

		m_perifocalX = positionDirection;
		m_perifocalY = m_perifocalZ.Cross(m_perifocalX);

		eccentricityTerm = 1.f;
	}
	else
	{
		m_perifocalX = eccentrictyVector / m_eccentricity;
		m_perifocalY = m_perifocalZ.Cross(m_perifocalX);

		if (m_eccentricity < (1.f - kEccentricityEpsilon))
		{
			m_type = Type::Ellipse;
			eccentricityTerm = 1.f - eccentricitySquared;
		}
		else if ((1.f + kEccentricityEpsilon) < m_eccentricity)
		{
			m_type = Type::Hyperbola;
			eccentricityTerm = eccentricitySquared - 1.f;
		}
		else
		{
			assert(false); // TODO - parabolic orbits ...
		}
	}

	m_semiMajor = m_parameter / eccentricityTerm; // Semi-major axis (a) = p / e'.
	m_semiMinor = m_semiMajor * Maths::Sqrt(eccentricityTerm); // Semi-minor axis (b) = a * sqrt(e').

	float periodSeconds = (kPIf * m_semiMajor * m_semiMinor) / m_angularMomentum; // Orbit period (t) = Pi * a * b / h.
	m_period = Time::Microseconds::Convert(periodSeconds);

	m_centreOffset = m_parameter / (1.f + m_eccentricity); // Signed distance (c) from occupied focus to the centre of the perifocal frame.
	switch (m_type)
	{
	case Type::Circle: // Fall through ...
	case Type::Ellipse:
		m_centreOffset -= m_semiMajor; // c = p / (1 + e) - a
		break;

	case Type::Parabola:
		assert(false); // TODO - parabolic orbits ...

	case Type::Hyperbola:
		m_centreOffset += m_semiMajor; // c = p / (1 + e) + a
		break;

	default:
		throw Exception(RESULT_CODE_UNRECOGNIZED, "Unrecognized orbit type");
	}

	m_inclination = Vector3::AngleBetweenUnitVectors(m_perifocalZ, kReferenceZ); // Inclination (i), the angle between the reference and perifocal Z-axes = acos(Zp DOT Zr)
	m_ascendingNodeDirection = m_perifocalZ.IsApproxParallel(kReferenceZ) ? m_perifocalX : kReferenceZ.Cross(m_perifocalZ).Normalized();

	m_rightAscension = Vector3::AngleBetweenUnitVectors(m_ascendingNodeDirection, kReferenceX);
	if (m_ascendingNodeDirection.Dot(kReferenceY) < 0.f)
		m_rightAscension = kPI2f - m_rightAscension;

	m_argumentPeriapsis = Vector3::AngleBetweenUnitVectors(m_ascendingNodeDirection, m_perifocalX);
	if (m_ascendingNodeDirection.Dot(m_perifocalY) < 0.f)
		m_argumentPeriapsis = kPI2f - m_argumentPeriapsis;

	// TODO - orientation ?
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

OrbitTestScript::OrbitTestScript() :
	ITestScript("Orbit")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

OrbitTestScript::~OrbitTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void OrbitTestScript::RunImpl(TestHandler & testHandler)
{
	Orbit::Elements elements;

	float gravityParameter = 1.f;
	Vector3 position = { 1.f, 0.f, 0.f };
	Vector3 velocity = { 0.f, 1.f, 0.f }; // Speed of circular orbit = sqrt(gravity parameter / orbit radius) -> sqrt(1 / 1) = 1

	elements.Compute(gravityParameter, position, velocity);

	testHandler.Assert(static_cast<unsigned>(elements.m_type), static_cast<unsigned>(Orbit::Type::Circle), "Circular orbit");

	//assert(false); // TODO - elements for circular orbit with period of 1 minute ...
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
