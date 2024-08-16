#ifndef NEBULA_TYPES_H
#define NEBULA_TYPES_H

namespace Nebula
{

using String = std::string;
using StringView = std::string_view;

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

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
using SharedPtr = std::shared_ptr<T>;

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline SharedPtr<T> MakeShared(T * ptr)
{
	return std::make_shared<T>(ptr);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename... TArgs>
inline SharedPtr<T> MakeShared(TArgs... args)
{
	return std::make_shared<T>(std::forward<TArgs>(args)...);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TYPES_H
