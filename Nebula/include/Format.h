#ifndef NEBULA_FORMAT_H
#define NEBULA_FORMAT_H

#include "NebulaString.h"

namespace Nebula
{

class Format
{
public:
	template<typename... TArgs>
	static String Fmt(char const* pFormatString, TArgs... args);
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename... TArgs>
inline String Format::Fmt(char const* pFormatString, TArgs... args)
{
	return std::format(pFormatString, std::forward<TArgs>(args)...);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_FORMAT_H
