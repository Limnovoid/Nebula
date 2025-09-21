#include "ExplicitArithmetic.h"

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
	struct ExplicitArithmeticTokenA {};
	struct ExplicitArithmeticTokenB {};

	using A = TExplicitArithmetic<float, ExplicitArithmeticTokenA>;
	using B = TExplicitArithmetic<float, ExplicitArithmeticTokenB>;

	// Checking compilation behaviour.
	{
		A a1;
		B b1;

		a1 = 1.f; // Error, explicit constructor.
		a1 = b1; // Error, explicit constructor.

		a1 = A(1.f);
		a1 = A(b1); // Error, cannot convert argument.

		a1 = a1 + b1; // Error, explicit constructor.
		a1 += b1; // Error, explicit constructor.

		a1 = a1 + b1.Get(); // Error, cannot convert argument.
		a1 += b1.Get(); // Error, cannot convert argument.

		a1 = a1 + A(b1.Get());
		a1 += A(b1.Get());

		a1 == a1;
		a1 == b1; // Error, cannot convert argument.
		a1 == 1.f;
		a1 != 1.f;
		1.f == a1;
		1.f != a1;

		a1 < b1; // Error, cannot convert argument.
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
		a1 + b1;
		a1 + 1.f;
		a1 += a1;
		a1 += b1;
		a1 += 1.f;
	}

	//static_assert(false); // TODO ...
	// operator+(TExplicitArithmetic const& rhs)
	// operator-(TExplicitArithmetic const& rhs)
	// operator*(TExplicitArithmetic const& rhs)
	// operator/(TExplicitArithmetic const& rhs)
	// operator+=(TExplicitArithmetic const& rhs)
	// operator-=(TExplicitArithmetic const& rhs)
	// operator*=(TExplicitArithmetic const& rhs)
	// operator/=(TExplicitArithmetic const& rhs)

}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------
