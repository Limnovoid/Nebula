#include "UiIo.h"

namespace Nebula
{

UiIo::UiIo(UiIo const& uiIo) :
	m_outputStream(uiIo.m_outputStream),
	m_inputStream(uiIo.m_inputStream)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

UiIo::UiIo(std::ostream & outputStream, std::istream & inputStream) :
	m_outputStream(outputStream),
	m_inputStream(inputStream)
{
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
