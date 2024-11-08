#include <jcfp.hpp>

using namespace jcfp;

std::expected<ClassFile, Error> ClassFile::parse(u1 *bytes, size_t maxlength)
{
	ClassFile cf = ClassFile();

	cf.magic = 123;
	
	return {cf};
}
