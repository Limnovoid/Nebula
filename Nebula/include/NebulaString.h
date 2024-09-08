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

	/// <param name="cstring">A c-style string with null terminator.</param>
	/// <returns>The length of the string including null terminator.</returns>
	static constexpr size_t GetLength(char const*const cstring);

	static char constexpr ToLower(char c);
	static char constexpr ToUpper(char c);

	constexpr String();
	constexpr String(String const& string);
	constexpr String(String && string);
	constexpr String(std::string const& string);
	constexpr String(char const*const cstr);
	constexpr String(char character);
	constexpr String(StringView stringView);

	constexpr String & operator=(String const& string);
	constexpr String & operator=(String && string);
	constexpr String & operator=(std::string const& string);
	constexpr String & operator=(char const*const cstr);
	constexpr String & operator=(char character);
	constexpr String & operator=(StringView stringView);
};

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr size_t String::GetLength(char const*const cstr)
{
	size_t i = 1;

	while ('\0' != cstr[i])
		++i;

	return i;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr char String::ToUpper(char c)
{
	if (('a' <= c) && (c <= 'z'))
		c -= ('a' - 'A');

	return c;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr char String::ToLower(char c)
{
	if (('A' <= c) && (c <= 'Z'))
		c += ('a' - 'A');

	return c;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr String::String()										: std::string("") {}
inline constexpr String::String(String const& string)					: std::string(string) {}
inline constexpr String::String(String && string)						: std::string(std::forward<String>(string)) {}
inline constexpr String::String(std::string const& string)				: std::string(string) {}
inline constexpr String::String(char const*const cstr)					: std::string(cstr) {}
inline constexpr String::String(char character)							: std::string(1, character) {}
inline constexpr String::String(StringView stringView)					: std::string(stringView.data(), stringView.length()) {}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr String & String::operator=(String const& string)		{ std::string::operator=(string); return *this; }
inline constexpr String & String::operator=(String && string)			{ std::string::operator=(std::forward<std::string>(string)); return *this; }
inline constexpr String & String::operator=(std::string const& string)	{ std::string::operator=(string); return *this; }
inline constexpr String & String::operator=(char const*const cstr)		{ std::string::operator=(cstr); return *this; }
inline constexpr String & String::operator=(char character)				{ std::string::operator=(character); return *this; }
inline constexpr String & String::operator=(StringView stringView)		{ std::string::operator=(stringView); return *this; }

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
