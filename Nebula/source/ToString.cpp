#include "ToString.h"

namespace Nebula
{

String ToString(bool const value)
{
	return (value ? "true" : "false");
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
