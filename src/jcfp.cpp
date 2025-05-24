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

std::expected<ClassFile, Error> ClassFile::parse(u1 *bytes)
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

	BufReader reader = BufReader(bytes);

	magic = reader.read_be<u4>();
	if (magic != CLASSFILE_MAGIC)
		return std::unexpected(Error { ErrorKind::WrongMagic, reader.prev_pos() });

	minor_version = reader.read_be<u2>();
	major_version = static_cast<MajorVersion>(reader.read_be<u2>());

	auto result = ConstantPool::parse(reader);
	if (!result.has_value())
		return std::unexpected(result.error());
	constant_pool = result.value();

	u2 interfaces_count = reader.read_be<u2>();
	for (u2 i = 0; i < interfaces_count; ++i) {
		u2 iface = reader.read_be<u2>();
		interfaces.push_back(iface);
	}

	u2 fields_count = reader.read_be<u2>();
	for (u2 i = 0; i < fields_count; ++i) {
		u2 access_flags = reader.read_be<u2>();
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
			access_flags, name_index, descriptor_index,
			std::move(attributes)
		});
	}

	u2 methods_count = reader.read_be<u2>();
	for (u2 i = 0; i < methods_count; ++i) {
		u2 access_flags = reader.read_be<u2>();
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
			access_flags, name_index, descriptor_index,
			std::move(attributes)
		});
	}

	u2 attributes_count = reader.read_be<u2>();
	for (u2 i = 0; i < attributes_count; ++i) {
		u2 attribute_name_index = reader.read_be<u2>();
		u4 attribute_length = reader.read_be<u4>();
		std::vector<u1> info = reader.read_bytes(attribute_length);
		attributes.push_back(AttributeInfo {
			attribute_name_index,
			std::move(info)
		});
	}
	

	return ClassFile(magic, minor_version, major_version, constant_pool,
	                 access_flags, this_class, super_class, interfaces,
	                 fields, methods, attributes);
}
