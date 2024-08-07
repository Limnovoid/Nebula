#ifndef NEBULA_ORION_METADATA_H
#define NEBULA_ORION_METADATA_H

#include "GetTypenameHelper.h"

namespace Nebula
{

namespace Orion
{

/// <summary>
/// Class for accessing type metadata in the debugger.
/// Intended for use in templated data structures to disambiguate their instantiations at runtime.
/// </summary>
/// <typeparam name="TValue">The value type.</typeparam>
/// <typeparam name="SSize">In statically-sized data structures, set to the size of the data structure.</typeparam>
template<typename TValue, size_t SSize = 0>
class Metadata
{
public:
	static const size_t						s_sizeof = sizeof(TValue);
	static const size_t						s_alignof = alignof(TValue);
	static const size_t						s_capacity = SSize;

	const GetTypenameHelper<TValue>			m_getTypenameHelper;
};

} // namespace Orion

} // namespace Nebula

#endif//NEBULA_ORION_METADATA_H
