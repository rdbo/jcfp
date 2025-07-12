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

#ifndef _JCFP_HPP_
#define _JCFP_HPP_

#include <vector>
#include <expected>
#include <optional>
#include "basetypes.hpp"
#include "constant_pool.hpp"
#include "attribute.hpp"
#include "error.hpp"

#define CLASSFILE_MAGIC 0xCAFEBABE
#define RELOCATE_INDEX(index, diff, from) { if (index >= from) index += diff; }

namespace jcfp {
	typedef struct {
		AccessFlags access_flags;
		u2 name_index;
		u2 descriptor_index;
		// u2 attributes_count;
		// attribute_info attributes[attributes_count];
		std::vector<AttributeInfo> attributes;
	} FieldInfo;

	typedef struct {
		AccessFlags access_flags;
		u2 name_index;
		u2 descriptor_index;
		// u2 attributes_count;
		// attribute_info attributes[attributes_count];
		std::vector<AttributeInfo> attributes;
	} MethodInfo;

	class ClassFile {
	public:
		u4 magic;
		u2 minor_version;
		MajorVersion major_version;
		// u2 constant_pool_count;
		// cp_info constant_pool[constant_pool_count - 1];
		ConstantPool constant_pool;
		AccessFlags access_flags;
		u2 this_class;
		u2 super_class;
		// u2 interfaces_count;
		// u2 interfaces[interfaces_count];
		std::vector<u2> interfaces;
		// u2 fields_count;
		// field_info fields[fields_count];
		std::vector<FieldInfo> fields;
		// u2 methods_count;
		// method_info methods[methods_count];
		std::vector<MethodInfo> methods;
		// u2 attributes_count;
		// attribute_info attributes[attributes_count];
		std::vector<AttributeInfo> attributes;
	public:
		ClassFile(u4 magic,
			  u2 minor_version,
			  MajorVersion major_version,
			  ConstantPool constant_pool,
			  AccessFlags access_flags,
			  u2 this_class,
			  u2 super_class,
			  std::vector<u2> interfaces,
			  std::vector<FieldInfo> fields,
			  std::vector<MethodInfo> methods,
			  std::vector<AttributeInfo> attributes)
		: magic(magic), minor_version(minor_version), major_version(major_version),
		  constant_pool(constant_pool), access_flags(access_flags), this_class(this_class),
		  super_class(super_class), interfaces(interfaces), fields(fields), methods(methods),
		  attributes(attributes)
		{}
	public:
		static std::expected<ClassFile, Error> parse(const u1 *bytes, size_t max_length);
		static inline std::expected<ClassFile, Error> parse(const std::vector<u1> &bytes) { return parse(bytes.data(), bytes.size()); }
		static inline std::expected<ClassFile, Error> parse(const u1 *bytes) { return parse(bytes, 0); }
		std::vector<u1> encode();
		void encode(ByteStream &stream);
		void relocate(int diff, u2 from);
	public:
		inline std::vector<std::string> get_attribute_names()
		{
			std::vector<std::string> attrs;
			for (auto &attr : attributes) {
				auto &cpi = constant_pool.get<ConstantPoolEntry::Utf8Info>(attr.attribute_name_index);
				attrs.push_back(cpi.bytes);
			}

			return attrs;
		}

		inline std::optional<AttributeInfo> find_attribute(const std::string &name)
		{
			for (auto &attr : attributes) {
				auto &cpi = constant_pool.get<ConstantPoolEntry::Utf8Info>(attr.attribute_name_index);
				if (cpi.bytes == name)
				       return attr;
			}

			return {};
		}
	};
}

#endif
