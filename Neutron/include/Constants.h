#ifndef NEUTRON_CONSTANTS_H
#define NEUTRON_CONSTANTS_H

#include "Vector3.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

constexpr double		kPI = 3.1415926535897932;
constexpr double		kPI2 = kPI * kPI;
constexpr float			kPIf = static_cast<float>(kPI);
constexpr float			kPI2f = static_cast<float>(kPI2);

constexpr double		kEps = std::numeric_limits<double>::epsilon();
constexpr float			kEpsf = std::numeric_limits<float>::epsilon();

constexpr float			kGravitational = 6.6743e-11f;		// Gravitational constant, units: m^3 kg^-1 s^-2
constexpr float			kEccentricityEpsilon = kEpsf;		// Minimum eccentricity of a non-circular orbit.
constexpr float			kMaximumScalingSpaceRadius = 0.25f;
constexpr float			kMinimumScalingSpaceRadius = 1000.f * kEpsf;
constexpr float			kMinimumRadiusOfInfluence = kMinimumScalingSpaceRadius;

constexpr Vector3		kReferenceX = { 1.f, 0.f, 0.f };
constexpr Vector3		kReferenceY = { 0.f, 1.f, 0.f };
constexpr Vector3		kReferenceZ = { 0.f, 0.f, 1.f };

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_CONSTANTS_H
