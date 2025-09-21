#ifndef NEUTRON_TYPES_H
#define NEUTRON_TYPES_H

#include "LengthUnits.h"
#include "Vector3.h"

namespace Neutron // --------------------------------------------------------------------------------------------------------------
{

using AbsVector3 = TVector3<Length::Absolute>;
using RelVector3 = TVector3<Length::Relative>;

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_TYPES_H
