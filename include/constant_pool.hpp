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

#ifndef _JCFP_CONSTANT_POOL_HPP_
#define _JCFP_CONSTANT_POOL_HPP_

#include <vector>
#include "basetypes.hpp"
#include "error.hpp"

namespace jcfp {
	class ConstantPoolEntry {
	public:
		enum Tag : u1 {
			/*
			 * Empty entries are used for:
			 *   - The first index of the constant pool, which is always invalid
			 *   - The additional unused indices of Double and Long entries
			 */
			Empty = 0,

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
	public:
		ConstantPoolEntry() : tag(Tag::Empty) {}
		ConstantPoolEntry(CONSTANT_Class_info info) : tag(Tag::CONSTANT_Class), class_info(info) {}

		ConstantPoolEntry(CONSTANT_Fieldref_info info) : tag(Tag::CONSTANT_Fieldref), fieldref_info(info) {}
		ConstantPoolEntry(CONSTANT_Methodref_info info) : tag(Tag::CONSTANT_Methodref), methodref_info(info) {}
		ConstantPoolEntry(CONSTANT_InterfaceMethodref_info info) : tag(Tag::CONSTANT_InterfaceMethodref), interface_methodref_info(info) {}

		ConstantPoolEntry(CONSTANT_String_info info) : tag(Tag::CONSTANT_String), string_info(info) {}
		ConstantPoolEntry(CONSTANT_Integer_info info) : tag(Tag::CONSTANT_Integer), integer_info(info) {}
		ConstantPoolEntry(CONSTANT_Float_info info) : tag(Tag::CONSTANT_Float), float_info(info) {}

		ConstantPoolEntry(CONSTANT_Long_info info) : tag(Tag::CONSTANT_Long), long_info(info) {}
		ConstantPoolEntry(CONSTANT_Double_info info) : tag(Tag::CONSTANT_Double), double_info(info) {}

		ConstantPoolEntry(CONSTANT_NameAndType_info info) : tag(Tag::CONSTANT_NameAndType), name_and_type_info(info) {}
		ConstantPoolEntry(CONSTANT_Utf8_info info) : tag(Tag::CONSTANT_Utf8), utf8_info(info) {}
		ConstantPoolEntry(CONSTANT_MethodHandle_info info) : tag(Tag::CONSTANT_MethodHandle), method_handle_info(info) {}
		ConstantPoolEntry(CONSTANT_InvokeDynamic_info info) : tag(Tag::CONSTANT_InvokeDynamic), invoke_dynamic_info(info) {}
	public:
		static std::expected<ConstantPoolEntry, Error> parse(u1 *bytes);

		inline bool is_wide_entry()
		{
			return tag == Tag::CONSTANT_Double || tag == Tag::CONSTANT_Long;
		}
	};

	class ConstantPool {
	private:
		/* Modifying the entries directly could cause issues, use the helper functions */
		std::vector<ConstantPoolEntry> entries;

	public:
		static std::expected<ConstantPool, Error> parse(u1 *bytes);

		/*
		 * The constant pool entries are defined as:
		 *
		 *     cp_info constant_pool[constant_pool_count-1];
		 *
		 * Which means that `constant_pool_count = total_entries + 1`
		 * This information is required for writing the ClassFile back
		 * into bytes
		 *
		 * Iteration goes from 1 to count - 1
		 */
		inline u2 count() {
			return entries.size() + 1;
		}

		/* Helper functions */
		inline ConstantPoolEntry::Tag get_tag(u2 index) {
			return entries[index].tag;
		}

		inline ConstantPoolEntry get_entry(u2 index) {
			return entries[index];
		}

		inline void push_entry(ConstantPoolEntry entry) {
			entries.push_back(entry);
			if (entry.is_wide_entry()) {
				entries.push_back(ConstantPoolEntry());
			}
		}

		inline ConstantPoolEntry pop_entry() {
			if (entries.size() > 1 && entries[entries.size() - 2].is_wide_entry()) {
				entries.pop_back(); // Last entry is Empty added due to a preceding wide entry,
			                            // will be skipped
			}

			ConstantPoolEntry last_entry = entries.back();
			entries.pop_back();

			return last_entry;
		}

		inline void insert_entry(u2 index, ConstantPoolEntry entry)
		{
			if (entry.is_wide_entry())
				entries.insert(entries.begin() + index, ConstantPoolEntry());

			entries.insert(entries.begin() + index, entry);
		}

		inline void remove_entry(u2 index)
		{
			ConstantPoolEntry entry = entries[index];

			entries.erase(entries.begin() + index);
			
			if (index > 1 && entries[index - 1].is_wide_entry()) {
				entries.erase(entries.begin() + (index - 1));
			} else if (entry.is_wide_entry()) {
				entries.erase(entries.begin() + index);
			}
		}
	};
}

#endif
