#ifndef NEBULA_UI_IO_H
#define NEBULA_UI_IO_H

namespace Nebula
{

class UiIo
{
public:
	UiIo(std::ostream & outputStream, std::istream & inputStream) :
		m_outputStream(outputStream),
		m_inputStream(inputStream)
	{
	}

	template<typename T>
	void Print(T const& t) const { m_outputStream << t; }

	template<typename T>
	void Get(T & t) const { m_inputStream >> t; }

private:
	std::ostream &		m_outputStream;
	std::istream &		m_inputStream;
};

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
