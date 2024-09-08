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
	void Get(T & t) const;

	template<typename T>
	void Get(StringView prompt, T & t) const;

	Result GetConfirmation(StringView prompt, bool const defaultPositive = false) const;

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
inline void UiIo::Get(T & t) const
{
	m_inputStream >> t;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline void UiIo::Get(StringView prompt, T & t) const
{
	m_outputStream << prompt << ": ";
	m_inputStream >> t;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Result UiIo::GetConfirmation(StringView prompt, bool const defaultPositive) const
{
	char confirmationChar;

	Get(Fmt::Format("{}? ({}|{}):", prompt, (defaultPositive ? 'Y' : 'y'), (defaultPositive ? 'n' : 'N')), confirmationChar);

	return ((String::ToUpper(confirmationChar) == 'Y') ? RESULT_CODE_SUCCESS : RESULT_CODE_FAILURE);
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
