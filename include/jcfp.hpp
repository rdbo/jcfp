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

	enum class Opcode {
		OP_nop                 = 0,
		OP_aconst_null         = 1,
		OP_iconst_m1           = 2,
		OP_iconst_0            = 3,
		OP_iconst_1            = 4,
		OP_iconst_2            = 5,
		OP_iconst_3            = 6,
		OP_iconst_4            = 7,
		OP_iconst_5            = 8,
		OP_lconst_0            = 9,
		OP_lconst_1            = 10,
		OP_fconst_0            = 11,
		OP_fconst_1            = 12,
		OP_fconst_2            = 13,
		OP_dconst_0            = 14,
		OP_dconst_1            = 15,
		OP_bipush              = 16,
		OP_sipush              = 17,
		OP_ldc                 = 18,
		OP_ldc_w               = 19,
		OP_ldc2_w              = 20,
		OP_iload               = 21,
		OP_lload               = 22,
		OP_fload               = 23,
		OP_dload               = 24,
		OP_aload               = 25,
		OP_iload_0             = 26,
		OP_iload_1             = 27,
		OP_iload_2             = 28,
		OP_iload_3             = 29,
		OP_lload_0             = 30,
		OP_lload_1             = 31,
		OP_lload_2             = 32,
		OP_lload_3             = 33,
		OP_fload_0             = 34,
		OP_fload_1             = 35,
		OP_fload_2             = 36,
		OP_fload_3             = 37,
		OP_dload_0             = 38,
		OP_dload_1             = 39,
		OP_dload_2             = 40,
		OP_dload_3             = 41,
		OP_aload_0             = 42,
		OP_aload_1             = 43,
		OP_aload_2             = 44,
		OP_aload_3             = 45,
		OP_iaload              = 46,
		OP_laload              = 47,
		OP_faload              = 48,
		OP_daload              = 49,
		OP_aaload              = 50,
		OP_baload              = 51,
		OP_caload              = 52,
		OP_saload              = 53,
		OP_istore              = 54,
		OP_lstore              = 55,
		OP_fstore              = 56,
		OP_dstore              = 57,
		OP_astore              = 58,
		OP_istore_0            = 59,
		OP_istore_1            = 60,
		OP_istore_2            = 61,
		OP_istore_3            = 62,
		OP_lstore_0            = 63,
		OP_lstore_1            = 64,
		OP_lstore_2            = 65,
		OP_lstore_3            = 66,
		OP_fstore_0            = 67,
		OP_fstore_1            = 68,
		OP_fstore_2            = 69,
		OP_fstore_3            = 70,
		OP_dstore_0            = 71,
		OP_dstore_1            = 72,
		OP_dstore_2            = 73,
		OP_dstore_3            = 74,
		OP_astore_0            = 75,
		OP_astore_1            = 76,
		OP_astore_2            = 77,
		OP_astore_3            = 78,
		OP_iastore             = 79,
		OP_lastore             = 80,
		OP_fastore             = 81,
		OP_dastore             = 82,
		OP_aastore             = 83,
		OP_bastore             = 84,
		OP_castore             = 85,
		OP_sastore             = 86,
		OP_pop                 = 87,
		OP_pop2                = 88,
		OP_dup                 = 89,
		OP_dup_x1              = 90,
		OP_dup_x2              = 91,
		OP_dup2                = 92,
		OP_dup2_x1             = 93,
		OP_dup2_x2             = 94,
		OP_swap                = 95,
		OP_iadd                = 96,
		OP_ladd                = 97,
		OP_fadd                = 98,
		OP_dadd                = 99,
		OP_isub                = 100,
		OP_lsub                = 101,
		OP_fsub                = 102,
		OP_dsub                = 103,
		OP_imul                = 104,
		OP_lmul                = 105,
		OP_fmul                = 106,
		OP_dmul                = 107,
		OP_idiv                = 108,
		OP_ldiv                = 109,
		OP_fdiv                = 110,
		OP_ddiv                = 111,
		OP_irem                = 112,
		OP_lrem                = 113,
		OP_frem                = 114,
		OP_drem                = 115,
		OP_ineg                = 116,
		OP_lneg                = 117,
		OP_fneg                = 118,
		OP_dneg                = 119,
		OP_ishl                = 120,
		OP_lshl                = 121,
		OP_ishr                = 122,
		OP_lshr                = 123,
		OP_iushr               = 124,
		OP_lushr               = 125,
		OP_iand                = 126,
		OP_land                = 127,
		OP_ior                 = 128,
		OP_lor                 = 129,
		OP_ixor                = 130,
		OP_lxor                = 131,
		OP_iinc                = 132,
		OP_i2l                 = 133,
		OP_i2f                 = 134,
		OP_i2d                 = 135,
		OP_l2i                 = 136,
		OP_l2f                 = 137,
		OP_l2d                 = 138,
		OP_f2i                 = 139,
		OP_f2l                 = 140,
		OP_f2d                 = 141,
		OP_d2i                 = 142,
		OP_d2l                 = 143,
		OP_d2f                 = 144,
		OP_i2b                 = 145,
		OP_i2c                 = 146,
		OP_i2s                 = 147,
		OP_lcmp                = 148,
		OP_fcmpl               = 149,
		OP_fcmpg               = 150,
		OP_dcmpl               = 151,
		OP_dcmpg               = 152,
		OP_ifeq                = 153,
		OP_ifne                = 154,
		OP_iflt                = 155,
		OP_ifge                = 156,
		OP_ifgt                = 157,
		OP_ifle                = 158,
		OP_if_icmpeq           = 159,
		OP_if_icmpne           = 160,
		OP_if_icmplt           = 161,
		OP_if_icmpge           = 162,
		OP_if_icmpgt           = 163,
		OP_if_icmple           = 164,
		OP_if_acmpeq           = 165,
		OP_if_acmpne           = 166,
		OP_goto                = 167,
		OP_jsr                 = 168,
		OP_ret                 = 169,
		OP_tableswitch         = 170,
		OP_lookupswitch        = 171,
		OP_ireturn             = 172,
		OP_lreturn             = 173,
		OP_freturn             = 174,
		OP_dreturn             = 175,
		OP_areturn             = 176,
		OP_return              = 177,
		OP_getstatic           = 178,
		OP_putstatic           = 179,
		OP_getfield            = 180,
		OP_putfield            = 181,
		OP_invokevirtual       = 182,
		OP_invokespecial       = 183,
		OP_invokestatic        = 184,
		OP_invokeinterface     = 185,
		OP_invokedynamic       = 186,
		OP_new                 = 187,
		OP_newarray            = 188,
		OP_anewarray           = 189,
		OP_arraylength         = 190,
		OP_athrow              = 191,
		OP_checkcast           = 192,
		OP_instanceof          = 193,
		OP_monitorenter        = 194,
		OP_monitorexit         = 195,
		OP_wide                = 196,
		OP_multianewarray      = 197,
		OP_ifnull              = 198,
		OP_ifnonnull           = 199,
		OP_goto_w              = 200,
		OP_jsr_w               = 201,
		OP_MAX                 = 201
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
