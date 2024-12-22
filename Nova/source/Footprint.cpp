#include "Footprint.h"

namespace Nova // -----------------------------------------------------------------------------------------------------------------
{

FootprintTestScript::FootprintTestScript() :
	ITestScript("Footprint")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

FootprintTestScript::~FootprintTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void FootprintTestScript::RunImpl(TestHandler & testHandler)
{
	constexpr Footprint footprint1;
	constexpr Footprint footprint2 = Footprint::Make<uint8_t>(8);
	constexpr Footprint footprint3 = Footprint::Make<uint16_t>(8);
	constexpr Footprint footprint4 = Footprint::Make<uint64_t>(32);

	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint1.Size(); }, 0, 0, "Footprint().Size()");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint2.Size(); }, 0, 8, "Footprint(8).Size()");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint3.Size(); }, 0, sizeof(uint16_t) * 8, "Footprint<uint16_t>(8).Size()");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint4.Size(); }, 0, sizeof(uint64_t) * 32, "Footprint<uint64_t>(32).Size()");

	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint1.Count(); }, 0, 0, "Footprint().Count()");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint2.Count(); }, 0, 8, "Footprint(8).Count()");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint3.Count(); }, 0, 8, "Footprint<uint16_t>(8).Count()");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint4.Count(); }, 0, 32, "Footprint<uint64_t>(32).Count()");

	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint1.Alignment(); }, 0, 0, "Footprint().Alignment()");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint2.Alignment(); }, 0, 1, "Footprint(8).Alignment()");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint3.Alignment(); }, 0, 2, "Footprint<uint16_t>(8).Alignment()");
	testHandler.Assert<size_t, int>([&](int) -> size_t { return footprint4.Alignment(); }, 0, 8, "Footprint<uint64_t>(32).Alignment()");
}

} // namespace Nova ---------------------------------------------------------------------------------------------------------------
