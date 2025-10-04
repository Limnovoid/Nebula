#include "Orbit.h"

#include "Constants.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

Orbit::Orbit() :
	m_sections(1),
	m_currentSectionIndex(0),
	m_trueAnomaly(0.0)
{
	assert(1 == m_sections.size());
	m_sections.front() = MakeUnique<Section>();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

void Orbit::Elements::Compute(Unit::Relative const& gravityParameter, RelVector3 const& position, RelVector3 const& velocity)
{
	using namespace Unit;

	// Angular momentum (H) = R x V.
	position.PreciseCross(velocity, m_angularMomentum, m_perifocalZ);

	RelVector3 angularMomentumVector = m_angularMomentum * m_perifocalZ;

	API_ASSERT_THROW(0.f < m_angularMomentum, RESULT_CODE_INVALID_PARAMETER,
		Fmt::Format("Angular momentum evaluated to zero from position ({}), velocity ({}).", position, velocity));

	RelVector3 positionDirection = position.Normalized();

	Vector3 eccentrictyVector = ((velocity.PreciseCross(angularMomentumVector) / gravityParameter) - positionDirection).Get();
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

	m_parameter = m_angularMomentum * m_angularMomentum / gravityParameter; // Orbit parameter (p) = H^2 / g.
	m_kVelocity = gravityParameter / m_angularMomentum;

	if (1.f <= m_eccentricity)
		m_kMass = gravityParameter * gravityParameter / Maths::Pow(m_angularMomentum, Unit::Relative(3));

	m_semiMajor = m_parameter / eccentricityTerm; // Semi-major axis (a) = p / e'.
	m_semiMinor = m_semiMajor * Maths::Sqrt(eccentricityTerm); // Semi-minor axis (b) = a * sqrt(e').

	float periodSeconds = kPIf * (m_semiMajor * m_semiMinor / m_angularMomentum).Get(); // Orbit period (t) = Pi * a * b / h.
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

	m_inclination = Radians(Vector3::AngleBetweenUnitVectors(m_perifocalZ.Get(), kReferenceZ)); // Inclination (i), the angle between the reference and perifocal Z-axes = acos(Zp DOT Zr)
	m_ascendingNodeDirection = m_perifocalZ.IsApproxParallel(RelVector3(kReferenceZ)) ? m_perifocalX : RelVector3(kReferenceZ).Cross(m_perifocalZ).Normalized();

	m_rightAscension = Radians(Vector3::AngleBetweenUnitVectors(m_ascendingNodeDirection.Get(), kReferenceX));
	if (m_ascendingNodeDirection.Dot(kReferenceY) < 0.f)
		m_rightAscension = kPI2f - m_rightAscension;

	m_argumentPeriapsis = Radians(RelVector3::AngleBetweenUnitVectors(m_ascendingNodeDirection, m_perifocalX).Get());
	if (m_ascendingNodeDirection.Dot(m_perifocalY) < 0.f)
		m_argumentPeriapsis = kPI2f - m_argumentPeriapsis;

	//m_orientation //TODO ...
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

	const Unit::Relative gravityParameter(1.f);
	const RelVector3 position = { 1.f, 0.f, 0.f };
	const RelVector3 velocity = { 0.f, 1.f, 0.f }; // Speed of circular orbit = sqrt(gravity parameter / orbit radius) -> sqrt(1 / 1) = 1

	elements.Compute(gravityParameter, position, velocity);

	testHandler.Assert(static_cast<unsigned>(elements.m_type), static_cast<unsigned>(Orbit::Type::Circle), "Circular orbit");

	//assert(false); // TODO - elements for circular orbit with period of 1 minute ...
}

} // namespace Neutron ------------------------------------------------------------------------------------------------------------
