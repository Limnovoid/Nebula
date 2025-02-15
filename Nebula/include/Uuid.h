#ifndef NEBULA_UUID
#define NEBULA_UUID

#include "Macros.h"

namespace Nebula // -----------------------------------------------------------------------------------------------------------------
{

class Uuid
{
public:
	Uuid();
	Uuid(Uuid const& other);

	uint64_t Get() const;

private:
	DEBUG_ONLY(static std::set<uint64_t>	s_uuids);

	uint64_t								m_value;
};

// --------------------------------------------------------------------------------------------------------------------------------

inline uint64_t Uuid::Get() const
{
	return m_value;
}

} // namespace Nebula ---------------------------------------------------------------------------------------------------------------

template<>
struct std::formatter<Nebula::Uuid> : std::formatter<uint64_t>
{
	auto format(Nebula::Uuid const uuid, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", uuid.Get());
	}
};

#endif//NEBULA_UUID
