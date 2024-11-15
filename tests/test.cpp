#include <jcfp.hpp>
#include <iostream>

using namespace jcfp;

int main()
{
	u1 buf[10240];
	FILE *f = fopen("Dummy.class", "r");
	if (!f) {
		std::cerr << "Failed to open 'Dummy.class'" << std::endl;
		return -1;
	}
	fread(buf, sizeof(buf), 1, f);
	fclose(f);

	auto result = ClassFile::parse(buf);
	if (!result.has_value()) {
		std::cerr << "Failed to parse ClassFile: " << static_cast<int>(result.error().kind) << " @ " << result.error().offset << std::endl;
		return -1;
	}

	ClassFile cf = result.value();
	std::cout << "CF magic: " << std::hex << cf.magic << std::dec << std::endl;
	std::cout << "CF minor: " << cf.minor_version << std::dec << std::endl;
	std::cout << "CF major: " << static_cast<u2>(cf.major_version) << std::dec << std::endl;

	return 0;
}
