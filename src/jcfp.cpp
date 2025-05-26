/*
 * Copyright (C) 2024  Rdbo
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3
 * as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <jcfp/jcfp.hpp>
#include <jcfp/utils.hpp>

using namespace jcfp;

std::expected<ClassFile, Error> ClassFile::parse(const u1 *bytes, size_t max_length)
{
        u4 magic;
        u2 minor_version;
        MajorVersion major_version;
        ConstantPool constant_pool;
        AccessFlags access_flags;
        u2 this_class;
        u2 super_class;
        std::vector<u2> interfaces;
        std::vector<FieldInfo> fields;
        std::vector<MethodInfo> methods;
        std::vector<AttributeInfo> attributes;

        BufReader reader = BufReader(bytes, max_length);
        LOG("Parsing ClassFile (bytes: %p, max_length: %lu)...", bytes, max_length);

        magic = reader.read_be<u4>();
        LOG("ClassFile magic: %X", magic);
        if (magic != CLASSFILE_MAGIC)
                return std::unexpected(Error { ErrorKind::WrongMagic, reader.prev_pos() });

        minor_version = reader.read_be<u2>();
        major_version = static_cast<MajorVersion>(reader.read_be<u2>());
        LOG("ClassFile version: %hu %hu", major_version, minor_version);

        auto result = ConstantPool::parse(reader);
        if (!result.has_value())
                return std::unexpected(result.error());
        constant_pool = result.value();

        access_flags = reader.read_be<AccessFlags>();
        this_class = reader.read_be<u2>();
        super_class = reader.read_be<u2>();
        LOG("Access flags: %hu", access_flags);
        LOG("This class: %hu", this_class);
        LOG("Super class: %hu", super_class);

        u2 interfaces_count = reader.read_be<u2>();
        LOG("Interfaces count: %hu", interfaces_count);
        for (u2 i = 0; i < interfaces_count; ++i) {
                u2 iface = reader.read_be<u2>();
                interfaces.push_back(iface);
        }

        u2 fields_count = reader.read_be<u2>();
        LOG("Fields count: %hu", fields_count);
        for (u2 i = 0; i < fields_count; ++i) {
                AccessFlags flags = reader.read_be<AccessFlags>(); // u2
                u2 name_index = reader.read_be<u2>();
                u2 descriptor_index = reader.read_be<u2>();

                std::vector<AttributeInfo> attributes;
                u2 attributes_count = reader.read_be<u2>();
                for (u2 j = 0; j < attributes_count; ++j) {
                        u2 attribute_name_index = reader.read_be<u2>();
                        u4 attribute_length = reader.read_be<u4>();
                        std::vector<u1> info = reader.read_bytes(attribute_length);
                        attributes.push_back(AttributeInfo {
                                attribute_name_index,
                                std::move(info)
                        });
                }

                fields.push_back(FieldInfo {
                        flags, name_index, descriptor_index,
                        std::move(attributes)
                });
        }

        u2 methods_count = reader.read_be<u2>();
        LOG("Methods count: %hu", methods_count);
        for (u2 i = 0; i < methods_count; ++i) {
                AccessFlags flags = reader.read_be<AccessFlags>();
                u2 name_index = reader.read_be<u2>();
                u2 descriptor_index = reader.read_be<u2>();

                std::vector<AttributeInfo> attributes;
                u2 attributes_count = reader.read_be<u2>();
                for (u2 j = 0; j < attributes_count; ++j) {
                        u2 attribute_name_index = reader.read_be<u2>();
                        u4 attribute_length = reader.read_be<u4>();
                        std::vector<u1> info = reader.read_bytes(attribute_length);
                        attributes.push_back(AttributeInfo {
                                attribute_name_index,
                                std::move(info)
                        });
                }

                methods.push_back(MethodInfo {
                        flags, name_index, descriptor_index,
                        std::move(attributes)
                });
        }

        u2 attributes_count = reader.read_be<u2>();
        LOG("Attributes count: %hu", attributes_count);
        for (u2 i = 0; i < attributes_count; ++i) {
                u2 attribute_name_index = reader.read_be<u2>();
                u4 attribute_length = reader.read_be<u4>();
                std::vector<u1> info = reader.read_bytes(attribute_length);
                attributes.push_back(AttributeInfo {
                        attribute_name_index,
                        std::move(info)
                });
        }

        LOG("ClassFile parsed successfully (offset: %lu)", reader.pos());

        return ClassFile(magic, minor_version, major_version, constant_pool,
                         access_flags, this_class, super_class, interfaces,
                         fields, methods, attributes);
}

std::vector<u1> ClassFile::encode()
{
        ByteStream stream = ByteStream();

        LOG("Encoding ClassFile to bytes...");

        stream.write_be(this->magic);
        stream.write_be(this->minor_version);
        stream.write_be(this->major_version);

        LOG("Encoding constant pool at offset: %lx", stream.size());
        std::vector<u1> encoded_constant_pool = constant_pool.encode();
        stream.write_bytes(encoded_constant_pool);
        LOG("Encoded constant pool size: %lu", encoded_constant_pool.size());

        stream.write_be(this->access_flags);
        stream.write_be(this->this_class);
        stream.write_be(this->super_class);

        LOG("Encoding interfaces at offset: %lx", stream.size());
        u2 interfaces_count = static_cast<u2>(this->interfaces.size());
        stream.write_be(interfaces_count);
        for (auto &interface : this->interfaces) {
                stream.write_be(interface);
        }

        // TODO: Abstract attribute handling into a helper

        LOG("Encoding fields at offset: %lx", stream.size());
        u2 fields_count = static_cast<u2>(this->fields.size());
        stream.write_be(fields_count);
        for (auto &field : this->fields) {
                stream.write_be(field.access_flags);
                stream.write_be(field.name_index);
                stream.write_be(field.descriptor_index);

                u2 attributes_count = static_cast<u2>(field.attributes.size());
                stream.write_be(attributes_count);
                for (auto &attribute : field.attributes) {
                        stream.write_be(attribute.attribute_name_index);

                        u4 attribute_length = attribute.info.size();
                        stream.write_be(attribute_length);
                        stream.write_bytes(attribute.info);
                }
        }

        LOG("Encoding methods at offset: %lx", stream.size());
        u2 methods_count = static_cast<u2>(this->methods.size());
        stream.write_be(methods_count);
        for (auto &method : this->methods) {
                stream.write_be(method.access_flags);
                stream.write_be(method.name_index);
                stream.write_be(method.descriptor_index);

                u2 attributes_count = static_cast<u2>(method.attributes.size());
                stream.write_be(attributes_count);
                for (auto &attribute : method.attributes) {
                        stream.write_be(attribute.attribute_name_index);

                        u4 attribute_length = attribute.info.size();
                        stream.write_be(attribute_length);
                        stream.write_bytes(attribute.info);
                }
        }

        LOG("Encoding attributes at offset: %lx", stream.size());
        u2 attributes_count = static_cast<u2>(this->attributes.size());
        stream.write_be(attributes_count);
        for (auto &attribute : this->attributes) {
                stream.write_be(attribute.attribute_name_index);

                u4 attribute_length = attribute.info.size();
                stream.write_be(attribute_length);
                stream.write_bytes(attribute.info);
        }

        LOG("ClassFile encoding finished successfully");

        return stream.collect();
}
