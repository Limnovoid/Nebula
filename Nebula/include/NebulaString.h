#ifndef NEBULA_STRING_H
#define NEBULA_STRING_H

namespace Nebula
{

using StringView = std::string_view;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class String : public std::string
{
public:
	/// <returns>True if the character is printable whitespace; false otherwise.</returns>
	static bool IsWhitespace(char const c);

	/// <summary> Get a view of the first word - unbroken sequence of non-whitespace characters - in the string. </summary>
	static StringView GetFirstWord(StringView string);

	String(char character);
	String(char const*const cstr);

	template<typename... TArgs>
	String(TArgs... args);
};

// --------------------------------------------------------------------------------------------------------------------------------

inline String::String(char character) :
	std::string(1, character)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline String::String(char const*const cstr) :
	std::string(cstr)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename... TArgs>
inline String::String(TArgs... args) :
	std::string(std::forward<TArgs>(args)...)
{
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline StringView MakeStringView(String const& string)
{
	return StringView(string.data(), string.length());
}

// --------------------------------------------------------------------------------------------------------------------------------

inline StringView MakeStringView(char const& character)
{
	return StringView(&character, 1);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_STRING_H
