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

#include "basetypes.hpp"
#include <stdexcept>

namespace jcfp {
	class BufReader {
	private:
		u1 *buffer;
		size_t prev_offset = 0;
		size_t offset = 0;
		size_t max_offset = 0;
	public:
		BufReader(u1 *buffer, size_t max_offset=0) : buffer(buffer), max_offset(max_offset) {}
	public:
		template <typename T>
		inline T read() // throws std::out_of_range
		{
			T *t = (T *)&this->buffer[this->offset];
			const auto next_offset = this->offset + sizeof(T);
			if (this->max_offset > 0 && next_offset > this->max_offset) {
				throw std::out_of_range(
					"Attempted to read from " + std::to_string(offset) + " to " +
					std::to_string(next_offset) + "(max offset: " + std::to_string(this->max_offset) + ")"
				);
			}

			this->prev_offset = this->offset;
			this->offset += sizeof(T);
			return *t;
		}

		template <typename T>
		inline T read_be()
		{
			T t = this->read<T>();
			T t_copy = t;
			u1 *bytes = (u1 *)&t;
			for (size_t i = 0; i < sizeof(T); ++i) {
				bytes[i] = ((u1 *)&t_copy)[sizeof(T) - i - 1];
			}

			return t;
		}

		inline std::vector<u1> read_bytes(size_t size)
		{
			std::vector<u1> bytes;

			for (size_t i = 0; i < size; ++i) {
				u1 byte = this->read<u1>();
				bytes.push_back(byte);
			}

			return bytes;
		}

		inline size_t prev_pos()
		{
			return this->prev_offset;
		}

		inline size_t pos()
		{
			return this->offset;
		}
	};
}

#endif
