#include "Orion.h"

#include <iostream>

int main(int argc, int** argv)
{
	using namespace Nebula::Orion;

	std::cout << GetTypenameHelper<double>()() << std::endl;
}
