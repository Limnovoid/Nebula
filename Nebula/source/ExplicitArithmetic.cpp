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

void ExplicitArithmeticTestScript::RunImpl(TestHandler & testHandler)
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

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

//const String ExplicitArithmeticTest::NAME = "ExplicitArithmetic";

// --------------------------------------------------------------------------------------------------------------------------------

void ExplicitArithmeticTest::Run() const
{
	constexpr A a0(0.f);
	constexpr A a1(1.f);
	constexpr A a2(2.f);
	constexpr A a3(3.f);
	constexpr A a4(4.f);
	constexpr A a5(5.f);
	constexpr A a6(6.f);

	AssertOperator("operator+", a1 + a1, Equal(a2));
	AssertOperator("operator-", a2 - a1, Equal(a1));
	AssertOperator("operator*", a2 * a2, Equal(a4));
	AssertOperator("operator/", a6 / a2, Equal(a3));

	AssertOperator("operator+ (T rhs)", a1 + 1.f, Equal(a2));
	AssertOperator("operator- (T rhs)", a2 - 1.f, Equal(a1));
	AssertOperator("operator* (T rhs)", a2 * 2.f, Equal(a4));
	AssertOperator("operator/ (T rhs)", a6 / 2.f, Equal(a3));

	A ax(1.f);
	AssertOperator("operator+=", ax += a3, Equal(a4));
	AssertOperator("operator-=", ax -= a2, Equal(a2));
	AssertOperator("operator*=", ax *= a3, Equal(a6));
	AssertOperator("operator/=", ax /= a2, Equal(a3));

	ax = A(1.f);
	AssertOperator("operator+= (T rhs)", ax += 3.f, Equal(a4));
	AssertOperator("operator-= (T rhs)", ax -= 2.f, Equal(a2));
	AssertOperator("operator*= (T rhs)", ax *= 3.f, Equal(a6));
	AssertOperator("operator/= (T rhs)", ax /= 2.f, Equal(a3));

	Assert("operator== #1", a0 == a0);
	Assert("operator== #2", !(a0 == a1));
	Assert("operator!= #1", a0 != a1);
	Assert("operator!= #2", !(a0 != a0));
	Assert("operator< #1", a0 < a1);
	Assert("operator< #2", !(a2 < a1));
	Assert("operator<= #1", a0 <= a0);
	Assert("operator<= #2", a0 <= a1);
	Assert("operator<= #3", !(a2 <= a1));
	Assert("operator> #1", a1 > a0);
	Assert("operator> #2", !(a1 > a2));
	Assert("operator>= #1", a0 >= a0);
	Assert("operator>= #2", a1 >= a0);
	Assert("operator>= #3", !(a1 >= a2));

	Assert("operator== (T rhs) #1", a0 == 0.f);
	Assert("operator== (T rhs) #2", !(a0 == 1.f));
	Assert("operator!= (T rhs) #1", a0 != 1.f);
	Assert("operator!= (T rhs) #2", !(a0 != 0.f));
	Assert("operator< (T rhs) #1", a0 < 1.f);
	Assert("operator< (T rhs) #2", !(a2 < 1.f));
	Assert("operator<= (T rhs) #1", a0 <= 0.f);
	Assert("operator<= (T rhs) #2", a0 <= 1.f);
	Assert("operator<= (T rhs) #3", !(a2 <= 1.f));
	Assert("operator> (T rhs) #1", a1 > 0.f);
	Assert("operator> (T rhs) #2", !(a1 > 2.f));
	Assert("operator>= (T rhs) #1", a0 >= 0.f);
	Assert("operator>= (T rhs) #2", a1 >= 0.f);
	Assert("operator>= (T rhs) #3", !(a1 >= 2.f));

	Assert("operator== (T lhs) #1", 0.f == a0);
	Assert("operator== (T lhs) #2", !(0.f == a1));
	Assert("operator!= (T lhs) #1", 0.f != a1);
	Assert("operator!= (T lhs) #2", !(0.f != a0));
	Assert("operator< (T lhs) #1", 0.f < a1);
	Assert("operator< (T lhs) #2", !(2.f < a1));
	Assert("operator<= (T lhs) #1", 0.f <= a0);
	Assert("operator<= (T lhs) #2", 0.f <= a1);
	Assert("operator<= (T lhs) #3", !(2.f <= a1));
	Assert("operator> (T lhs) #1", 1.f > a0);
	Assert("operator> (T lhs) #2", !(1.f > a2));
	Assert("operator>= (T lhs) #1", 0.f >= a0);
	Assert("operator>= (T lhs) #2", 1.f >= a0);
	Assert("operator>= (T lhs) #3", !(1.f >= a2));

	AssertOperator("operator- (negate) #1", -a1, Equal(A(-1.f)));
	AssertOperator("operator- (negate) #2", -a1, NotEqual(a1));
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
