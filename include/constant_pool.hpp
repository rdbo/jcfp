#ifndef _JCFP_CONSTANT_POOL_HPP_
#define _JCFP_CONSTANT_POOL_HPP_

#include "basetypes.hpp"
#include <vector>

namespace jcfp {
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
}

#endif
