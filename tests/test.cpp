#include <jcfp/jcfp.hpp>
#include <iostream>

using namespace jcfp;

int main()
{
        u1 buf[10240];
        size_t size;
        FILE *f = fopen("Dummy.class", "r");
        if (!f) {
                std::cerr << "Failed to open 'Dummy.class'" << std::endl;
                return -1;
        }
        fread(buf, sizeof(buf), 1, f);
        size = ftell(f) + 1;
        fclose(f);

        auto result = ClassFile::parse(buf, size);
        if (!result.has_value()) {
                std::cerr << "Failed to parse ClassFile: " << static_cast<int>(result.error().kind) << " @ " << result.error().offset << std::endl;
                return -1;
        }

        ClassFile cf = result.value();
        std::cout << "CF magic: " << std::hex << cf.magic << std::dec << std::endl;
        std::cout << "CF minor: " << cf.minor_version << std::dec << std::endl;
        std::cout << "CF major: " << static_cast<u2>(cf.major_version) << std::dec << std::endl;

        std::cout << "constant pool count: " << cf.constant_pool.count() << std::endl;
        // for (auto &cpi : cf.constant_pool.get_entries()) {
        //         std::cout << "TAG: " << static_cast<u2>(cpi.tag) << std::endl;
        // }
        
        std::vector<u1> encoded = cf.encode();
        std::cout << "Encoded CF size: " << encoded.size() << std::endl;
        std::cout << "Original size: " << size << std::endl;

        bool verify = encoded == std::vector<u1>(buf, buf + size);
        std::cout << "CF Verify: " << (verify ? "OK" : "BAD") << std::endl;

        if (!verify) {
                std::cout << std::endl << std::endl << std::endl <<
                             "Reparsing class to identify issues through logs..." <<
                             std::endl << std::endl << std::endl;

                ClassFile::parse(encoded);
        }

        return 0;
}
