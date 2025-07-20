#ifndef NEBULA_NEEDS_INITIALIZATION_HELPER_H
#define NEBULA_NEEDS_INITIALIZATION_HELPER_H

#include "Macros.h"
#include "Result.h"

namespace Nebula // --------------------------------------------------------------------------------------------------------------
{

class NeedsInitializationHelper
{
public:
	NeedsInitializationHelper(bool ownerNeedsInitialization = true);

	void Set(bool ownerNeedsInitialization = true);

	/// <returns>Not Initialized if owner needs initialization, otherwise Success.</returns>
	Result Check();

private:
	bool	m_ownerNeedsInitialization;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline NeedsInitializationHelper::NeedsInitializationHelper(bool ownerNeedsInitialization) :
	m_ownerNeedsInitialization(ownerNeedsInitialization)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void NeedsInitializationHelper::Set(bool ownerNeedsInitialization)
{
	m_ownerNeedsInitialization = ownerNeedsInitialization;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline Result NeedsInitializationHelper::Check()
{
	assert(!m_ownerNeedsInitialization);

	return m_ownerNeedsInitialization ? RESULT_CODE_NOT_INITIALIZED : RESULT_CODE_SUCCESS;
}

} // namespace Nebula ------------------------------------------------------------------------------------------------------------

#endif//NEBULA_NEEDS_INITIALIZATION_HELPER_H
