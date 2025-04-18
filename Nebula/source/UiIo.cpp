#include "UiIo.h"

#include "Macros.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------------------------------------------------------

Result UiIo::GetConfirmation(StringView prompt, bool const defaultPositive) const
{
	char confirmationChar = (defaultPositive ? 'y' : 'n');

	Result result = Get(confirmationChar, Fmt::Format("{}?", prompt), true);

	IF_OR_RESULT_CODE(SUCCESS, UNCHANGED, == , result)
	{
		if ('Y' == String::ToUpper(confirmationChar))
			return RESULT_CODE_SUCCESS;
	}

	return RESULT_CODE_FAILURE;
}

// --------------------------------------------------------------------------------------------------------------------------------

void UiIo::Newline(size_t const numNewlines) const
{
	for (size_t i = 0; i < numNewlines; ++i)
		m_outputStream << '\n';
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
