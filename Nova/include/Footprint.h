#ifndef NOVA_FOOTPRINT_H
#define NOVA_FOOTPRINT_H

#include "NebulaTypes.h"
#include "Macros.h"
#include "TestHandler.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

using namespace Nebula;

// --------------------------------------------------------------------------------------------------------------------------------

/// <summary> Class for describing memory blocks based on the type for which they are allocated. </summary>
class Footprint
{
public:
	/// <summary> Get an instance of Footprint initialized with the given count and alignment of type T. </summary>
	/// <typeparam name="T"> The type to use as the basis for the stored alignment. </typeparam>
	/// <param name="count"> The count to store in the constructed object. </param>
	/// <returns> The constructed Footprint object. </returns>
	template<typename T>
	static constexpr Footprint Make(size_t count);

	constexpr Footprint();
	constexpr Footprint(size_t count, size_t typeSize, size_t alignment);

	/// <returns> Size of block in bytes. </returns>
	constexpr size_t Size() const;

	/// <returns> Size of block as a number of elements of type T. </returns>
	constexpr size_t Count() const;

	/// <returns> Size of type T. </returns>
	constexpr size_t TypeSize() const;

	/// <returns> Alignment of type T. </returns>
	constexpr size_t Alignment() const;

	constexpr bool IsInitialized() const;

	constexpr operator bool() const;

private:
	size_t	m_count;
	size_t	m_typeSize;
	size_t	m_alignment;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline constexpr Footprint Footprint::Make(size_t count)
{
	ASSERT(0 != count);

	return Footprint(count, sizeof(T), alignof(T));
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr Footprint::Footprint() :
	m_count(0),
	m_typeSize(0),
	m_alignment(0)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr Footprint::Footprint(size_t count, size_t typeSize, size_t alignment) :
	m_count(count),
	m_typeSize(typeSize),
	m_alignment(alignment)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr size_t Footprint::Size() const
{
	return m_typeSize * m_count;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr size_t Footprint::Count() const
{
	return m_count;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr size_t Footprint::TypeSize() const
{
	return m_typeSize;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline constexpr size_t Footprint::Alignment() const
{
	return m_alignment;
}

// --------------------------------------------------------------------------------------------------------------------------------

constexpr bool Footprint::IsInitialized() const
{
	return m_count != 0;
}

// --------------------------------------------------------------------------------------------------------------------------------

constexpr Footprint::operator bool() const
{
	return IsInitialized();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class FootprintTestProgram : public ITestProgram
{
public:
	FootprintTestProgram();
	virtual ~FootprintTestProgram();

protected:
	virtual void RunImpl(TestHandler & testHandler) override;
};

} // namespace Nova ---------------------------------------------------------------------------------------------------------------

#endif//NOVA_FOOTPRINT_H
