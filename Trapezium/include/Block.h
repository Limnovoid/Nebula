#ifndef NEBULA_TRAPEZIUM_BLOCK_H
#define NEBULA_TRAPEZIUM_BLOCK_H

namespace Nebula
{

namespace Trapezium
{

using byte_t = uint8_t;

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

/// <summary>
/// Wrapper class for a statically allocated block of memory. Equivalent to an array of type T with S elements.
/// </summary>
template<size_t S, typename T = byte_t>
class Block
{
public:
	Block();
	Block(Block const& rhs) = delete;
	Block(Block && rhs) = delete;

private:
	byte_t alignas(T)			m_bytes[S * sizeof(T)];
};

// --------------------------------------------------------------------------------------------------------------------------------

template<size_t S, typename T>
Block<S, T>::Block() :
	m_bytes{ 0 }
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------



} // namespace Trapezium ----------------------------------------------------------------------------------------------------------

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TRAPEZIUM_BLOCK_H
