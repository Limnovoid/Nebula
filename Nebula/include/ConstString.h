#ifndef NEBULA_CONST_STRING_H
#define NEBULA_CONST_STRING_H

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

template<size_t NSize>
class ConstString
{
public:
	constexpr ConstString(char c = '\0');
	constexpr ConstString(char const(& string)[NSize]);

	constexpr char & operator[](size_t index);
	constexpr char const& operator[](size_t index) const;

	template<size_t NRhsSize>
	constexpr auto operator+(ConstString<NRhsSize> const& rhs) const;

	template<size_t NRhsSize>
	constexpr auto operator+(char const(& rhs)[NRhsSize]) const;

private:
	template<size_t NSize>
	friend std::ostream & operator<<(std::ostream & os, ConstString<NSize> const& constString);

	std::array<char, NSize>	m_cstr;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NSize>
inline constexpr ConstString<NSize>::ConstString(char c) :
	m_cstr{ '\0' }
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NSize>
inline constexpr ConstString<NSize>::ConstString(char const(& string)[NSize])
{
	for (size_t i = 0; i < NSize; ++i)
		m_cstr[i] = string[i];
}

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NSize>
inline constexpr char & ConstString<NSize>::operator[](size_t index)
{
	return m_cstr[index];
}

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NSize>
inline constexpr char const& ConstString<NSize>::operator[](size_t index) const
{
	return m_cstr[index];
}

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NSize>
template<size_t NRhsSize>
inline constexpr auto ConstString<NSize>::operator+(ConstString<NRhsSize> const& rhs) const
{
	constexpr size_t NFinalSize = NSize + NRhsSize - 1;

	ConstString<NFinalSize> finalString;

	for (size_t i = 0; i < (NSize - 1); ++i)
		finalString[i] = m_cstr[i];

	for (size_t i = 0; i < (NRhsSize - 1); ++i)
		finalString[NSize - 1 + i] = rhs[i];

	return finalString;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NSize>
template<size_t NRhsSize>
inline constexpr auto ConstString<NSize>::operator+(char const(&rhs)[NRhsSize]) const
{
	return *this + ConstString<NRhsSize>(rhs);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NSize>
inline std::ostream & operator<<(std::ostream & os, ConstString<NSize> const& constString)
{
	return (os << constString.m_cstr.data());
}

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NSize1, size_t NSize2>
inline constexpr auto operator+(char const(&string)[NSize1], ConstString<NSize2> const& constString)
{
	return ConstString(string) + constString;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NSize>
constexpr size_t StringLength(char const(&)[NSize])
{
	return NSize;
}

// --------------------------------------------------------------------------------------------------------------------------------

/// <summary> Find the length of the string, excluding the null terminator. </summary>
constexpr size_t NumChar(char const*const string)
{
	size_t length = 0;

	while ('\0' != string[length])
		++length;

	return length;
}

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t... NSizes>
constexpr auto ConstCat(char const(&... strings)[NSizes])
{
	constexpr size_t NFinalSize = (... + NSizes) - sizeof...(NSizes) + 1;

	ConstString<NFinalSize> finalString;

	size_t iTotal = 0;
	for (auto string : { strings... })
		for (size_t i = 0; i < NumChar(string); ++i, ++iTotal)
			finalString[iTotal] = string[i];

	return finalString;
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_CONST_STRING_H
