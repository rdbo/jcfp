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

 #include <jcfp.hpp>
 #include "utils.hpp"

using namespace jcfp;

std::expected<ConstantPool, Error> parse_constant_pool(BufReader &reader)
{
	std::vector<ConstantPoolEntry> entries;

	u2 constant_pool_count = reader.read_be<u2>();
	if (constant_pool_count == 0)
		return ConstantPool(entries);

	// The first entry is always an empty tag
	entries.push_back(ConstantPoolEntry());

	for (u2 i = 1; i < constant_pool_count - 1; ++i) {
		auto result = parse_constant_pool_entry(reader);
		if (!result.has_value())
			return std::unexpected(result.error());

		entries.push_back(result.value());
	}

	return ConstantPool(entries);
}

std::expected<ConstantPoolEntry, Error> parse_constant_pool_entry(BufReader &reader)
{
	using Tag = ConstantPoolEntry::Tag;
	using EntryVariant = ConstantPoolEntry::EntryVariant;

	u1 tag = reader.read<u1>();
	EntryVariant info;

	switch (tag) {
		case Tag::Empty:
		{
			info = ConstantPoolEntry::Empty_info {};

			break;
		}
		case Tag::CONSTANT_Class:
		{
			ConstantPoolEntry::CONSTANT_Class_info val;
			val.name_index = reader.read_be<u2>();
			info = val;

			break;
		}
		case Tag::CONSTANT_Fieldref:
		{
			ConstantPoolEntry::CONSTANT_Fieldref_info val;
			val.class_index = reader.read_be<u2>();
			val.name_and_type_index = reader.read_be<u2>();
			info = val;

			break;
		}
		case Tag::CONSTANT_Methodref:
		{
			ConstantPoolEntry::CONSTANT_Methodref_info val;
			val.class_index = reader.read_be<u2>();
			val.name_and_type_index = reader.read_be<u2>();
			info = val;

			break;
		}
		case Tag::CONSTANT_InterfaceMethodref:
		{
			ConstantPoolEntry::CONSTANT_InterfaceMethodref_info val;
			val.class_index = reader.read_be<u2>();
			val.name_and_type_index = reader.read_be<u2>();
			info = val;

			break;
		}
		case Tag::CONSTANT_String:
		{
			ConstantPoolEntry::CONSTANT_String_info val;
			val.string_index = reader.read_be<u2>();
			info = val;

			break;
		}
		case Tag::CONSTANT_Integer:
		{
			ConstantPoolEntry::CONSTANT_Integer_info val;
			val.bytes = reader.read_be<u4>();
			info = val;

			break;
		}
		case Tag::CONSTANT_Float:
		{
			ConstantPoolEntry::CONSTANT_Float_info val;
			val.bytes = reader.read_be<u4>();
			info = val;

			break;
		}
		case Tag::CONSTANT_Long:
		{
			ConstantPoolEntry::CONSTANT_Long_info val;
			val.high_bytes = reader.read_be<u4>();
			val.low_bytes = reader.read_be<u4>();
			info = val;

			break;
		}
		case Tag::CONSTANT_Double:
		{
			ConstantPoolEntry::CONSTANT_Double_info val;
			val.high_bytes = reader.read_be<u4>();
			val.low_bytes = reader.read_be<u4>();
			info = val;

			break;
		}
		case Tag::CONSTANT_NameAndType:
		{
			ConstantPoolEntry::CONSTANT_NameAndType_info val;
			val.name_index = reader.read_be<u2>();
			val.descriptor_index = reader.read_be<u2>();
			info = val;

			break;
		}
		case Tag::CONSTANT_Utf8:
		{
			ConstantPoolEntry::CONSTANT_Utf8_info val;

			u2 length = reader.read_be<u2>();
			std::vector<u1> bytes = reader.read_bytes(length);
			std::string str = std::string(bytes.begin(), bytes.end());
			val.bytes = str;
			info = val;

			break;
		}
		case Tag::CONSTANT_MethodHandle:
		{
			ConstantPoolEntry::CONSTANT_MethodHandle_info val;
			val.reference_kind = reader.read_be<u1>();
			val.reference_index = reader.read_be<u2>();
			info = val;

			break;
		}
		case Tag::CONSTANT_MethodType:
		{
			ConstantPoolEntry::CONSTANT_MethodType_info val;
			val.descriptor_index = reader.read_be<u2>();
			info = val;

			break;
		}
		case Tag::CONSTANT_InvokeDynamic:
		{
			ConstantPoolEntry::CONSTANT_InvokeDynamic_info val;
			val.bootstrap_method_attr_index = reader.read_be<u2>();
			val.name_and_type_index = reader.read_be<u2>();
			info = val;

			break;
		}
	}

	return ConstantPoolEntry(info);
}


std::expected<ConstantPoolEntry, Error> ConstantPoolEntry::parse(u1 *bytes)
{
	BufReader reader = BufReader(bytes);
	return parse_constant_pool_entry(reader);
}

std::expected<ConstantPool, Error> ConstantPool::parse(u1 *bytes)
{
	BufReader reader = BufReader(bytes);

	return parse_constant_pool(reader);
}
