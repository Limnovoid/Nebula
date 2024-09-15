#ifndef NEBULA_UI_IO_H
#define NEBULA_UI_IO_H

#include "NebulaTypes.h"
#include "Result.h"
#include "Format.h"

namespace Nebula
{

class UiIo
{
public:
	UiIo(UiIo const& uiIo);
	UiIo(std::ostream & outputStream, std::istream & inputStream);

	template<typename T>
	void Print(T const& t) const;

	template<typename T>
	void Get(T & t, StringView prompt = "") const;

	Result GetConfirmation(StringView prompt = "Are you sure", bool const defaultPositive = false) const;

	void Newline(size_t const numNewlines = 1) const;

private:
	std::ostream &		m_outputStream;
	std::istream &		m_inputStream;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline void UiIo::Print(T const& t) const
{
	m_outputStream << t;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline void UiIo::Get(T & t, StringView prompt) const
{
	if ("" != prompt)
		m_outputStream << prompt << " > ";

	m_inputStream >> t;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Result UiIo::GetConfirmation(StringView prompt, bool const defaultPositive) const
{
	char confirmationChar;

	Get(confirmationChar, Fmt::Format("{}? ({}|{})", prompt, (defaultPositive ? 'Y' : 'y'), (defaultPositive ? 'n' : 'N')));

	return ((String::ToUpper(confirmationChar) == 'Y') ? RESULT_CODE_SUCCESS : RESULT_CODE_FAILURE);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void UiIo::Newline(size_t const numNewlines) const
{
	for (size_t i = 0; i < numNewlines; ++i)
		m_outputStream << '\n';
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline UiIo const& operator<<(UiIo const& uiIo, T const& t)
{
	uiIo.Print(t);

	return uiIo;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline UiIo const& operator>>(UiIo const& uiIo, T & t)
{
	uiIo.Get(t);

	return uiIo;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_UI_IO_H
