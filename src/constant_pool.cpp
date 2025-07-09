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
 #include <format>

using namespace jcfp;
using Tag = ConstantPoolEntry::Tag;
using EntryVariant = ConstantPoolEntry::EntryVariant;

std::expected<ConstantPool, Error> ConstantPool::parse(BufReader &reader)
{
	std::vector<ConstantPoolEntry> entries;

	LOG("Parsing constant pool (offset: %lu)...", reader.pos());

	u2 constant_pool_count = reader.read_be<u2>();
	if (constant_pool_count == 0) {
		LOG("Empty constant pool");
		return ConstantPool(entries);
	}
	LOG("Constant pool count: %hu", constant_pool_count);

	// The first entry is always an empty tag
	entries.push_back(ConstantPoolEntry());

	for (u2 i = 1; i <= constant_pool_count - 1; ++i) {
		auto result = ConstantPoolEntry::parse(reader);
		if (!result.has_value()) {
			ERR("Failed to parse constant pool entry '%hu'", i);
			return std::unexpected(result.error());
		}

		LOG("New constant pool entry parsed (%hu): %s", i, result.value().to_string().c_str());

		auto entry = result.value();
		entries.push_back(entry);

		/* From: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
		 * """
		 * All 8-byte constants take up two entries in the constant_pool table of the
		 * class file. If a CONSTANT_Long_info or CONSTANT_Double_info structure is the
		 * item in the constant_pool table at index n, then the next usable item in the
		 * pool is located at index n+2. The constant_pool index n+1 must be valid but
		 * is considered unusable.
		 *
		 * In retrospect, making 8-byte constants take two constant pool entries was a poor choice.
		 * """
		 */
		if (entry.tag == Tag::Long || entry.tag == Tag::Double) {
			entries.push_back(ConstantPoolEntry());
			++i;
		}
	}

	LOG("Constant pool parsed successfully (offset: %lu, entries: %lu)", reader.pos(), entries.size());

	return ConstantPool(entries);
}

std::expected<ConstantPoolEntry, Error> ConstantPoolEntry::parse(BufReader &reader)
{
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


std::expected<ConstantPoolEntry, Error> ConstantPoolEntry::parse(const u1 *bytes, size_t max_length)
{
	BufReader reader = BufReader(bytes, max_length);
	return ConstantPoolEntry::parse(reader);
}

std::expected<ConstantPool, Error> ConstantPool::parse(const u1 *bytes, size_t max_length)
{
	BufReader reader = BufReader(bytes, max_length);

	return ConstantPool::parse(reader);
}

std::vector<u1>	ConstantPool::encode()
{
	ByteStream stream = ByteStream();

	u2 constant_pool_count = this->count();
	stream.write_be(constant_pool_count);

	for (auto &entry : this->entries) {
		stream.write_bytes(entry.encode());
	}

	return stream.collect();        
}

std::vector<u1>	ConstantPoolEntry::encode()
{
	ByteStream stream = ByteStream();

	if (this->tag == Tag::Empty)
		return stream.collect();

	stream.write_be(this->tag);

	switch (this->tag) {
	case Tag::Class: {
		ClassInfo &info = this->get<ClassInfo>();
		stream.write_be(info.name_index);
		break;
	}
	case Tag::Fieldref: {
		FieldrefInfo &info = this->get<FieldrefInfo>();
		stream.write_be(info.class_index);
		stream.write_be(info.name_and_type_index);
		break;
	}
	case Tag::Methodref: {
		MethodrefInfo &info = this->get<MethodrefInfo>();
		stream.write_be(info.class_index);
		stream.write_be(info.name_and_type_index);
		break;
	}
	case Tag::InterfaceMethodref: {
		InterfaceMethodrefInfo &info = this->get<InterfaceMethodrefInfo>();
		stream.write_be(info.class_index);
		stream.write_be(info.name_and_type_index);
		break;
	}
	case Tag::String: {
		StringInfo &info = this->get<StringInfo>();
		stream.write_be(info.string_index);
		break;
	}
	case Tag::Integer: {
		IntegerInfo &info = this->get<IntegerInfo>();
		stream.write_be(info.bytes);
		break;
	}
	case Tag::Float: {
		FloatInfo &info = this->get<FloatInfo>();
		stream.write_be(info.bytes);
		break;
	}
	case Tag::Long: {
		LongInfo &info = this->get<LongInfo>();
		stream.write_be(info.high_bytes);
		stream.write_be(info.low_bytes);
		break;
	}
	case Tag::Double: {
		DoubleInfo &info = this->get<DoubleInfo>();
		stream.write_be(info.high_bytes);
		stream.write_be(info.low_bytes);
		break;
	}
	case Tag::NameAndType: {
		NameAndTypeInfo &info = this->get<NameAndTypeInfo>();
		stream.write_be(info.name_index);
		stream.write_be(info.descriptor_index);
		break;
	}
	case Tag::Utf8: {
		Utf8Info &info = this->get<Utf8Info>();

		u2 length = info.bytes.size();
		stream.write_be(length);
		for (char b : info.bytes) {
			stream.write(b);
		}
		break;
	}
	case Tag::MethodHandle: {
		MethodHandleInfo &info = this->get<MethodHandleInfo>();
		stream.write_be(info.reference_kind);
		stream.write_be(info.reference_index);
		break;
	}
	case Tag::MethodType: {
		MethodTypeInfo &info = this->get<MethodTypeInfo>();
		stream.write_be(info.descriptor_index);
		break;
	}
	case Tag::InvokeDynamic: {
		InvokeDynamicInfo &info = this->get<InvokeDynamicInfo>();
		stream.write_be(info.bootstrap_method_attr_index);
		stream.write_be(info.name_and_type_index);

		break;
	}
	default:
		ERR("Failed to encode tag '%u' (unknown)", this->tag);
		break;
	}

	return stream.collect();
}

void ConstantPool::relocate(int diff, u2 from)
{
	for (auto &entry : this->get_entries()) {
		entry.relocate(diff, from);
	}
}

void ConstantPoolEntry::relocate(int diff, u2 from)
{
	switch (this->tag) {
	case Tag::Empty:
	case Tag::Integer:
	case Tag::Float:
	case Tag::Long:
	case Tag::Double:
	case Tag::Utf8:
		break;
	case Tag::Class: {
		ClassInfo &info = this->get<ClassInfo>();
		RELOCATE_INDEX(info.name_index, diff, from);
		break;
	}
	case Tag::Fieldref: {
		FieldrefInfo &info = this->get<FieldrefInfo>();
		RELOCATE_INDEX(info.class_index, diff, from);
		RELOCATE_INDEX(info.name_and_type_index, diff, from);
		break;
	}
	case Tag::Methodref: {
		MethodrefInfo &info = this->get<MethodrefInfo>();
		RELOCATE_INDEX(info.class_index, diff, from);
		RELOCATE_INDEX(info.name_and_type_index, diff, from);
		break;
	}
	case Tag::InterfaceMethodref: {
		InterfaceMethodrefInfo &info = this->get<InterfaceMethodrefInfo>();
		RELOCATE_INDEX(info.class_index, diff, from);
		RELOCATE_INDEX(info.name_and_type_index, diff, from);
		break;
	}
	case Tag::String: {
		StringInfo &info = this->get<StringInfo>();
		RELOCATE_INDEX(info.string_index, diff, from);
		break;
	}
	case Tag::NameAndType: {
		NameAndTypeInfo &info = this->get<NameAndTypeInfo>();
		RELOCATE_INDEX(info.name_index, diff, from);
		RELOCATE_INDEX(info.descriptor_index, diff, from);
		break;
	}
	case Tag::MethodHandle: {
		MethodHandleInfo &info = this->get<MethodHandleInfo>();
		RELOCATE_INDEX(info.reference_index, diff, from);
		break;
	}
	case Tag::MethodType: {
		MethodTypeInfo &info = this->get<MethodTypeInfo>();
		RELOCATE_INDEX(info.descriptor_index, diff, from);
		break;
	}
	case Tag::InvokeDynamic: {
		InvokeDynamicInfo &info = this->get<InvokeDynamicInfo>();
		RELOCATE_INDEX(info.bootstrap_method_attr_index, diff, from);
		RELOCATE_INDEX(info.name_and_type_index, diff, from);
		break;
	}
	default:
		ERR("Failed to relocate tag '%u' (unknown)", this->tag);
		break;
	}
}

std::string ConstantPoolEntry::to_string()
{
	using Tag = ConstantPoolEntry::Tag;
	using EntryVariant = ConstantPoolEntry::EntryVariant;

	std::string fmt;
	switch(this->tag) {
	case Tag::Empty: fmt = "Empty {}"; break;
	case Tag::Class: fmt = std::format("Class {{ name_index: {} }}", this->get<ClassInfo>().name_index); break;
	case Tag::Fieldref: fmt = std::format("Fieldref {{ class_index: {}, name_and_type_index: {} }}", this->get<FieldrefInfo>().class_index, this->get<FieldrefInfo>().name_and_type_index); break;
	case Tag::Methodref: fmt = std::format("Methodref {{ class_index: {}, name_and_type_index: {} }}", this->get<MethodrefInfo>().class_index, this->get<MethodrefInfo>().name_and_type_index); break;
	case Tag::InterfaceMethodref: fmt = std::format("InterfaceMethodref {{ class_index: {}, name_and_type_index: {} }}", this->get<InterfaceMethodrefInfo>().class_index, this->get<InterfaceMethodrefInfo>().name_and_type_index); break;
	case Tag::String: fmt = std::format("String {{ string_index: {} }}", this->get<StringInfo>().string_index); break;
	case Tag::Integer: fmt = std::format("Integer {{ bytes: {} }}", this->get<IntegerInfo>().bytes); break;
	case Tag::Float: fmt = std::format("Float {{ bytes: {} }}", this->get<FloatInfo>().bytes); break;
	case Tag::Long: fmt = std::format("Long {{ high_bytes: {:#x}, low_bytes: {:#x} }}", this->get<LongInfo>().high_bytes, this->get<LongInfo>().low_bytes); break;
	case Tag::Double: fmt = std::format("Double {{ high_bytes: {:#x}, low_bytes: {:#x} }}", this->get<DoubleInfo>().high_bytes, this->get<DoubleInfo>().low_bytes); break;
	case Tag::NameAndType: fmt = std::format("NameAndType {{ name_index: {}, descriptor_index: {} }}", this->get<NameAndTypeInfo>().name_index, this->get<NameAndTypeInfo>().descriptor_index); break;
	case Tag::Utf8: fmt = std::format("Utf8 {{ bytes: \"{}\" }}", this->get<Utf8Info>().bytes); break;
	case Tag::MethodHandle: fmt = std::format("MethodHandle {{ reference_kind: {}, reference_index: {} }}", this->get<MethodHandleInfo>().reference_kind, this->get<MethodHandleInfo>().reference_index); break;
	case Tag::MethodType: fmt = std::format("MethodType {{ descriptor_index: {} }}", this->get<MethodTypeInfo>().descriptor_index); break;
	case Tag::InvokeDynamic: fmt = std::format("InvokeDynamic {{ bootstrap_method_attr_index: {}, name_and_type_index: {} }}", this->get<InvokeDynamicInfo>().bootstrap_method_attr_index, this->get<InvokeDynamicInfo>().name_and_type_index); break;
	default: fmt = "<Unknown (tag: " + std::to_string(this->tag) + ")>"; break;
	}
	return fmt;
}
