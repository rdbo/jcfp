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
#include "basetypes.hpp"
#include "constant_pool.hpp"
#include "error.hpp"

namespace jcfp {
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
	public:
		static std::expected<ClassFile, Error> parse(u1 *bytes, size_t maxlength = 0);
	};
}

#endif
