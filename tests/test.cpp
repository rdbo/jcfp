#include <jcfp/jcfp.hpp>
#include <iostream>
#include <algorithm>

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
        size = ftell(f);
        fclose(f);

        std::cout << "Standard test";
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
                f = fopen("Encoded.class", "w");
                fwrite(encoded.data(), 1, encoded.size(), f);
                fclose(f);
                std::cout << "Bad class dumped to 'Encoded.class'" << std::endl;

                for (size_t i = 0; i < std::min(encoded.size(), size); ++i) {
                        if (encoded[i] != buf[i]) {
                                std::cout << "Mismatch at offset: " << (void *)i << std::endl;
                                break;
                        }
                }

                std::cout << std::endl << std::endl << std::endl <<
                             "Reparsing class to identify issues..." <<
                             std::endl << std::endl << std::endl;

                ClassFile::parse(encoded);
                return 1;
        }

        std::cout << std::endl;

        std::cout << "Constant pool test" << std::endl;
        auto entry = cf.constant_pool.remove_entry(2);
        cf.constant_pool.insert_entry(2, entry);
        encoded = cf.encode();
        verify = encoded == std::vector<u1>(buf, buf + size);
        std::cout << "CF Verify: " << (verify ? "OK" : "BAD") << std::endl;
        if (!verify) {
                f = fopen("Encoded.class", "w");
                fwrite(encoded.data(), 1, encoded.size(), f);
                fclose(f);
                std::cout << "Bad class dumped to 'Encoded.class'" << std::endl;

                for (size_t i = 0; i < std::min(encoded.size(), size); ++i) {
                        if (encoded[i] != buf[i]) {
                                std::cout << "Mismatch at offset: " << (void *)i << std::endl;
                                break;
                        }
                }

                std::cout << std::endl << std::endl << std::endl <<
                             "Reparsing class to identify issues..." <<
                             std::endl << std::endl << std::endl;

                ClassFile::parse(encoded);
                return 1;
        }


        std::cout << std::endl;

        std::cout << "Constant pool relocation test" << std::endl;
        cf.constant_pool.insert_entry(2, entry);
        cf.constant_pool.relocate(+1, 2);
        cf.constant_pool.remove_entry(2);
        cf.constant_pool.relocate(-1, 2);
        encoded = cf.encode();
        verify = encoded == std::vector<u1>(buf, buf + size);
        std::cout << "CF Verify: " << (verify ? "OK" : "BAD") << std::endl;
        if (!verify) {
                f = fopen("Encoded.class", "w");
                fwrite(encoded.data(), 1, encoded.size(), f);
                fclose(f);
                std::cout << "Bad class dumped to 'Encoded.class'" << std::endl;

                for (size_t i = 0; i < std::min(encoded.size(), size); ++i) {
                        if (encoded[i] != buf[i]) {
                                std::cout << "Mismatch at offset: " << (void *)i << std::endl;
                                break;
                        }
                }

                std::cout << std::endl << std::endl << std::endl <<
                             "Reparsing class to identify issues..." <<
                             std::endl << std::endl << std::endl;

                ClassFile::parse(encoded);
                return 1;
        }

        return 0;
}
