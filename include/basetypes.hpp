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

#ifndef _JCFP_BASETYPES_HPP_
#define _JCFP_BASETYPES_HPP_

#include <cstdint>

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
}

#endif
