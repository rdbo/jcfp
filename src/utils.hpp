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

#ifndef _JCFP_UTILS_HPP
#define _JCFP_UTILS_HPP

#include <basetypes.hpp>

using namespace jcfp;

inline void from_big_endian(u1 *from, u1 *to, size_t size)
{
	for (size_t i = 0; i < size; ++i) {
		to[i] = from[size - 1 - i];
	}
}

template <typename T>
inline T from_big_endian(T &buf)
{
	T result;

	from_big_endian(reinterpret_cast<u1 *>(&buf), reinterpret_cast<u1 *>(&result), sizeof(T));

	return result;
}

#endif
