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

std::expected<ConstantPool, Error> ConstantPool::parse(BufReader &reader)
{
        std::vector<ConstantPoolEntry> entries;

        u2 constant_pool_count = reader.read_be<u2>();
        if (constant_pool_count == 0)
                return ConstantPool(entries);

        // The first entry is always an empty tag
        entries.push_back(ConstantPoolEntry());

        for (u2 i = 1; i < constant_pool_count - 1; ++i) {
                auto result = ConstantPoolEntry::parse(reader);
                if (!result.has_value())
                        return std::unexpected(result.error());

                entries.push_back(result.value());
        }

        return ConstantPool(entries);
}

std::expected<ConstantPoolEntry, Error> ConstantPoolEntry::parse(BufReader &reader)
{
        using Tag = ConstantPoolEntry::Tag;
        using EntryVariant = ConstantPoolEntry::EntryVariant;

        u1 tag = reader.read<u1>();
        EntryVariant info;

        switch (tag) {
                case Tag::Empty:
                {
                        info = ConstantPoolEntry::EmptyInfo {};

                        break;
                }
                case Tag::Class:
                {
                        ConstantPoolEntry::ClassInfo val;
                        val.name_index = reader.read_be<u2>();
                        info = val;

                        break;
                }
                case Tag::Fieldref:
                {
                        ConstantPoolEntry::FieldrefInfo val;
                        val.class_index = reader.read_be<u2>();
                        val.name_and_type_index = reader.read_be<u2>();
                        info = val;

                        break;
                }
                case Tag::Methodref:
                {
                        ConstantPoolEntry::MethodrefInfo val;
                        val.class_index = reader.read_be<u2>();
                        val.name_and_type_index = reader.read_be<u2>();
                        info = val;

                        break;
                }
                case Tag::InterfaceMethodref:
                {
                        ConstantPoolEntry::InterfaceMethodrefInfo val;
                        val.class_index = reader.read_be<u2>();
                        val.name_and_type_index = reader.read_be<u2>();
                        info = val;

                        break;
                }
                case Tag::String:
                {
                        ConstantPoolEntry::StringInfo val;
                        val.string_index = reader.read_be<u2>();
                        info = val;

                        break;
                }
                case Tag::Integer:
                {
                        ConstantPoolEntry::IntegerInfo val;
                        val.bytes = reader.read_be<u4>();
                        info = val;

                        break;
                }
                case Tag::Float:
                {
                        ConstantPoolEntry::FloatInfo val;
                        val.bytes = reader.read_be<u4>();
                        info = val;

                        break;
                }
                case Tag::Long:
                {
                        ConstantPoolEntry::LongInfo val;
                        val.high_bytes = reader.read_be<u4>();
                        val.low_bytes = reader.read_be<u4>();
                        info = val;

                        break;
                }
                case Tag::Double:
                {
                        ConstantPoolEntry::DoubleInfo val;
                        val.high_bytes = reader.read_be<u4>();
                        val.low_bytes = reader.read_be<u4>();
                        info = val;

                        break;
                }
                case Tag::NameAndType:
                {
                        ConstantPoolEntry::NameAndTypeInfo val;
                        val.name_index = reader.read_be<u2>();
                        val.descriptor_index = reader.read_be<u2>();
                        info = val;

                        break;
                }
                case Tag::Utf8:
                {
                        ConstantPoolEntry::Utf8Info val;

                        u2 length = reader.read_be<u2>();
                        std::vector<u1> bytes = reader.read_bytes(length);
                        std::string str = std::string(bytes.begin(), bytes.end());
                        val.bytes = str;
                        info = val;

                        break;
                }
                case Tag::MethodHandle:
                {
                        ConstantPoolEntry::MethodHandleInfo val;
                        val.reference_kind = reader.read_be<u1>();
                        val.reference_index = reader.read_be<u2>();
                        info = val;

                        break;
                }
                case Tag::MethodType:
                {
                        ConstantPoolEntry::MethodTypeInfo val;
                        val.descriptor_index = reader.read_be<u2>();
                        info = val;

                        break;
                }
                case Tag::InvokeDynamic:
                {
                        ConstantPoolEntry::InvokeDynamicInfo val;
                        val.bootstrap_method_attr_index = reader.read_be<u2>();
                        val.name_and_type_index = reader.read_be<u2>();
                        info = val;

                        break;
                }
        }

        return ConstantPoolEntry(info);
}


std::expected<ConstantPoolEntry, Error> ConstantPoolEntry::parse(u1 *bytes, size_t max_length)
{
        BufReader reader = BufReader(bytes, max_length);
        return ConstantPoolEntry::parse(reader);
}

std::expected<ConstantPool, Error> ConstantPool::parse(u1 *bytes, size_t max_length)
{
        BufReader reader = BufReader(bytes, max_length);

        return ConstantPool::parse(reader);
}
