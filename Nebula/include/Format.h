#ifndef NEBULA_FORMAT_H
#define NEBULA_FORMAT_H

#include "NebulaString.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class Fmt
{
public:
	template<typename... TArgs>
	using FormatString = std::format_string<TArgs...>;

	template<typename... TArgs>
	static String Format(FormatString<TArgs...> formatString, TArgs... args);
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename... TArgs>
inline String Fmt::Format(FormatString<TArgs...> formatString, TArgs... args)
{
	return std::vformat(formatString.get(), std::make_format_args(args...));
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_FORMAT_H
