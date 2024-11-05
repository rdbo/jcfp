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

#include <cstdint>
#include <vector>

namespace jcfp {
	typedef uint8_t u1;
	typedef uint16_t u2;
	typedef uint32_t u4;

	/* General use access flags */
	enum class AccessFlags : u2 {
		ACC_PUBLIC       = 0x0001,
		ACC_PRIVATE      = 0x0002,
		ACC_PROTECTED    = 0x0004,
		ACC_STATIC       = 0x0008,
		ACC_FINAL        = 0x0010,
		ACC_SYNCHRONIZED = 0x0020,
		ACC_SUPER        = 0x0020,
		ACC_VOLATILE     = 0x0040,
		ACC_BRIDGE       = 0x0040,
		ACC_TRANSIENT    = 0x0080,
		ACC_VARARGS      = 0x0080,
		ACC_NATIVE       = 0x0100,
		ACC_INTERFACE    = 0x0200,
		ACC_ABSTRACT     = 0x0400,
		ACC_STRICT       = 0x0800,
		ACC_SYNTHETIC    = 0x1000,
		ACC_ANNOTATION   = 0x2000,
		ACC_ENUM         = 0x4000,
		ACC_MODULE       = 0x8000
	};

	class ConstantPoolEntry {
	public:
		enum class Tag : u1 {
			/*
			 * Null entries are used for:
			 *   - The first index of the constant pool, which is always invalid
			 *   - The additional unused indices of Double and Long entries
			 */
			NullEntry = 0,

			/* Standard constant pool entries */
			CONSTANT_Class              = 7,
			CONSTANT_Fieldref           = 9,
			CONSTANT_Methodref          = 10,
			CONSTANT_InterfaceMethodref = 11,
			CONSTANT_String             = 8,
			CONSTANT_Integer            = 3,
			CONSTANT_Float              = 4,
			CONSTANT_Long               = 5,
			CONSTANT_Double             = 6,
			CONSTANT_NameAndType        = 12,
			CONSTANT_Utf8               = 1,
			CONSTANT_MethodHandle       = 15,
			CONSTANT_MethodType         = 16,
			CONSTANT_InvokeDynamic      = 18,
		};

		typedef struct {
			u2 name_index;
		} CONSTANT_Class_info;

		/*
		 * NOTE: Even though Fieldref, Methodref and InterfaceMethodref
		 *       are the same, we are following the ClassFile Format
		 *       specification, and they are separate structs there.
		 *
		 *       See: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
		 */
		typedef struct {
			u2 class_index;
			u2 name_and_type_index;
		} CONSTANT_Fieldref_info;

		typedef struct {
			u2 class_index;
			u2 name_and_type_index;
		} CONSTANT_Methodref_info;

		typedef struct {
			u2 class_index;
			u2 name_and_type_index;
		} CONSTANT_InterfaceMethodref_info;

		typedef struct {
			u2 string_index;
		} CONSTANT_String_info;

		typedef struct {
			u4 bytes;
		} CONSTANT_Integer_info;

		typedef struct {
			u4 bytes;
		} CONSTANT_Float_info;

		typedef struct {
			u4 high_bytes;
			u4 low_bytes;
		} CONSTANT_Long_info;

		typedef struct {
			u4 high_bytes;
			u4 low_bytes;
		} CONSTANT_Double_info;

		typedef struct {
			u2 name_index;
			u2 descriptor_index;
		} CONSTANT_NameAndType_info;

		typedef struct {
			u2 length;
			u1 bytes[];
		} CONSTANT_Utf8_info;

		typedef struct {
			u1 reference_kind;
			u2 reference_index;
		} CONSTANT_MethodHandle_info;

		typedef struct {
			u2 descriptor_index;
		} CONSTANT_MethodType_info;

		typedef struct {
			u2 bootstrap_method_attr_index;
			u2 name_and_type_index;
		} CONSTANT_InvokeDynamic_info;
	public:
		Tag tag;
		union {
			CONSTANT_Class_info class_info;

			CONSTANT_Fieldref_info fieldref_info;
			CONSTANT_Methodref_info methodref_info;
			CONSTANT_InterfaceMethodref_info interface_methodref_info;

			CONSTANT_String_info string_info;
			CONSTANT_Integer_info integer_info;
			CONSTANT_Float_info float_info;

			CONSTANT_Long_info long_info;
			CONSTANT_Double_info double_info;

			CONSTANT_NameAndType_info name_and_type_info;
			CONSTANT_Utf8_info utf8_info;
			CONSTANT_MethodHandle_info method_handle_info;
			CONSTANT_InvokeDynamic_info invoke_dynamic_info;
		};
	};

	class ConstantPool {
	public:
		std::vector<ConstantPoolEntry> entries;

	public:
		/*
		 * The constant pool entries are defined as:
		 *
		 *     cp_info constant_pool[constant_pool_count-1];
		 *
		 * Which means that `constant_pool_count = total_entries + 1`
		 * This information is required for writing the ClassFile back
		 * into bytes
		 */
		inline u2 count() {
			return entries.size() + 1;
		}
	};

	class ClassFile {
	public:
		u4 magic;
		u2 minor_version;
		u2 major_version;
		ConstantPool constant_pool;
		AccessFlags access_flags;
		u2 this_class;
		u2 super_class;
		std::vector<u2> interfaces;
		std::vector<u2> fields;
		std::vector<u2> methods;
		std::vector<u2> attributes;
	};
}

#endif
