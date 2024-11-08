#include <jcfp.hpp>
#include <iostream>

using namespace jcfp;

int main()
{
	ClassFile cf = ClassFile::parse(nullptr).value();

	std::cout << "CF magic: " << cf.magic << std::endl;

	return 0;
}
