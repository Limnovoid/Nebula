#include "ExplicitArithmetic.h"

#include "TestHandler.h"

namespace
{
	struct ExplicitArithmeticTokenA {};
	struct ExplicitArithmeticTokenB {};

	class A : public Nebula::TExplicitArithmetic<float, A>
	{
	public:
		A() = default;
		explicit constexpr A(const float value) : Nebula::TExplicitArithmetic<float, A>(value) {}
	};

	class B : public Nebula::TExplicitArithmetic<float, B>
	{
	public:
		B() = default;
		explicit constexpr B(const float value) : Nebula::TExplicitArithmetic<float, B>(value) {}
	};
}

template<>
struct std::formatter<A> : std::formatter<float>
{
	auto format(A const& value, std::format_context & ctx) const
	{
		return std::format_to(ctx.out(), "{}", value.Get());
	}
};

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

ExplicitArithmeticTestScript::ExplicitArithmeticTestScript() :
	ITestScript("Explicit Arithmetic")
{
}

// --------------------------------------------------------------------------------------------------------------------------------

ExplicitArithmeticTestScript::~ExplicitArithmeticTestScript()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

void ExplicitArithmeticTestScript::RunImpl(Nebula::TestHandler & testHandler)
{
	// Checking compilation behaviour.
	{
		A a1;
		B b1;

		//a1 = 1.f; // Error, explicit constructor.
		//a1 = b1; // Error, explicit constructor.

		a1 = A(1.f);
		//a1 = A(b1); // Error, cannot convert argument.

		//a1 = a1 + b1; // Error, explicit constructor.
		//a1 += b1; // Error, explicit constructor.

		//a1 = a1 + b1.Get(); // Error, cannot convert argument.
		a1 += b1.Get();

		a1 = a1 + A(b1.Get());
		a1 += A(b1.Get());

		bool dummy;

		dummy = a1 == a1;
		//dummy = a1 == b1; // Error, cannot convert argument.
		dummy = a1 == 1.f;
		dummy = a1 != 1.f;
		dummy = 1.f == a1;
		dummy = 1.f != a1;

		//a1 < b1; // Error, cannot convert argument.
		a1 < a1;
		a1 > a1;
		a1 <= a1;
		a1 >= a1;
		a1 < 1.f;
		a1 > 1.f;
		a1 <= 1.f;
		a1 >= 1.f;
		1.f < a1;
		1.f > a1;
		1.f <= a1;
		1.f >= a1;

		a1 + a1;
		//a1 + b1; // Error, cannot convert argument.
		a1 + 1.f;
		a1 += a1;
		//a1 += b1; // Error, cannot convert argument.
		a1 += 1.f;

		1.f + a1;
		1.f - a1;
		1.f * a1;
		1.f / a1;
	}

	constexpr A a0(0.f);
	constexpr A a1(1.f);
	constexpr A a2(2.f);
	constexpr A a3(3.f);
	constexpr A a4(4.f);
	constexpr A a5(5.f);
	constexpr A a6(6.f);

	// operator+(TExplicitArithmetic const& rhs)
	testHandler.Assert(a1 + a1, a2, "operator+");

	// operator-(TExplicitArithmetic const& rhs)
	testHandler.Assert(a1 - a1, a0, "operator-");

	// operator*(TExplicitArithmetic const& rhs)
	testHandler.Assert(a2 * a3, a6, "operator*");

	// operator/(TExplicitArithmetic const& rhs)
	testHandler.Assert(a6 / a3, a2, "operator/");

	// operator+=(TExplicitArithmetic const& rhs)
	A a_1(1.f);
	a_1 += a3;
	testHandler.Assert(a_1, a4, "operator+=");

	// operator-=(TExplicitArithmetic const& rhs)
	a_1 -= a2;
	testHandler.Assert(a_1, a2, "operator-=");

	// operator*=(TExplicitArithmetic const& rhs)
	a_1 *= a3;
	testHandler.Assert(a_1, a6, "operator*=");

	// operator/=(TExplicitArithmetic const& rhs)
	a_1 /= a2;
	testHandler.Assert(a_1, a3, "operator*=");
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
