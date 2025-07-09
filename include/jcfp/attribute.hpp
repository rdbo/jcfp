/*
 * Copyright (C) 2025  Rdbo
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

#ifndef _JCFP_ATTRIBUTE_HPP_
#define _JCFP_ATTRIBUTE_HPP_

#include <vector>
#include <variant>
#include <string>
#include "utils.hpp"
#include "basetypes.hpp"
#include "error.hpp"

namespace jcfp {
	class AttributeInfo {
	public:
		u2 attribute_name_index;
		// u4 attribute_length;
		std::vector<u1> info;
	public:
		AttributeInfo() {}
		AttributeInfo(u2 attribute_name_index, std::vector<u1> info) :
			attribute_name_index(attribute_name_index), info(info) {}
	public:
		static AttributeInfo parse(BufReader &reader);
		static AttributeInfo parse(const u1 *bytes, size_t max_length=0);
		static inline AttributeInfo parse(const std::vector<u1> &bytes) { return parse(bytes.data(), bytes.size()); }
		std::vector<u1> encode();
		void encode(ByteStream &stream);
	};
}

#endif
