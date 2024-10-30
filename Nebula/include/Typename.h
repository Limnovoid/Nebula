#ifndef NEBULA_TYPENAME_H
#define NEBULA_TYPENAME_H

#include "ConstString.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

template<typename... T>
struct Typename
{
};

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

#define REGISTER_TYPENAME(type)		\
template<>							\
struct Typename<type>				\
{									\
	static constexpr auto Get()		\
	{								\
		return ConstString(#type);	\
	}								\
}

// ---------------------------------------------------------------------------------------------------------------------------------

//REGISTER_TYPENAME(int);			// Clashes with int32_t
REGISTER_TYPENAME(float);
REGISTER_TYPENAME(double);
REGISTER_TYPENAME(long double);
REGISTER_TYPENAME(uint8_t);
REGISTER_TYPENAME(uint16_t);
REGISTER_TYPENAME(uint32_t);
REGISTER_TYPENAME(uint64_t);
REGISTER_TYPENAME(int8_t);
REGISTER_TYPENAME(int16_t);
REGISTER_TYPENAME(int32_t);
REGISTER_TYPENAME(int64_t);

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TYPENAME_H
