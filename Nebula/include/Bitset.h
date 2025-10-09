#ifndef NEBULA_BITSET_H
#define NEBULA_BITSET_H

#include "Maths.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

template<size_t NBits>
class Bitset : std::bitset<NBits>
{
	using Base = std::bitset<NBits>;

public:
	Bitset();

	template<CUInt UInt>
	Bitset(UInt uInt);

	template<CInt Int>
	Int To() const;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NBits>
Bitset<NBits>::Bitset() :
	Base()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NBits>
template<CUInt UInt>
Bitset<NBits>::Bitset(UInt uInt) :
	Base(uInt)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t NBits>
template<CInt Int>
Int Bitset<NBits>::To() const
{
	return static_cast<Int>(Base::to_ulong());
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_BITSET_H
