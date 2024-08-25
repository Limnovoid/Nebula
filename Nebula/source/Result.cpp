#include "Result.h"

namespace Nebula
{

Result::Result() :
	m_resultCode(RESULT_CODE_SUCCESS)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

Result::Result(ResultCode resultCode) :
	m_resultCode(resultCode)
{
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
