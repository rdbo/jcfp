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

#ifdef DEBUG
#       define LOG(fmt, ...) printf("[JCFP] " fmt, ##__VA_ARGS__)
#       define ERR(fmt, ...) printf("[JCFP] [ERROR] " fmt, ##__VA_ARGS__)
#else
#       define LOG(fmt, ...)
#       define ERR(fmt, ...)
#endif

namespace jcfp {
        class BufReader {
        private:
                const u1 *buffer;
                size_t prev_offset = 0;
                size_t offset = 0;
                size_t max_length = 0;
        public:
                BufReader(const u1 *buffer, size_t max_length=0) : buffer(buffer), max_length(max_length) {}
        public:
                template <typename T>
                inline T read() // throws std::out_of_range
                {
                        T *t = (T *)&this->buffer[this->offset];
                        const auto next_offset = this->offset + sizeof(T);
                        if (this->max_length > 0 && next_offset > this->max_length) {
                                throw std::out_of_range(
                                        "Attempted to read from " + std::to_string(offset) + " to " +
                                        std::to_string(next_offset) + " (max offset: " + std::to_string(this->max_length) + ")"
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

        class ByteStream {
        private:
                std::vector<u1> bytes;
        public:
                std::vector<u1> collect()
                {
                        std::vector<u1> buf = this->bytes;
                        this->bytes = {};
                        return buf;
                }
        public:
                void write_bytes(const std::vector<u1> &buf)
                {
                        this->bytes.insert(this->bytes.end(), buf.begin(), buf.end());
                }

                void write_bytes(const u1 *buf, size_t size)
                {
                        this->bytes.insert(this->bytes.end(), buf, &buf[size]);
                }

                template <typename T>
                inline void write(const T &value)
                {
                        this->write_bytes(&value, &value + 1);
                }

                template <typename T>
                inline void write_be(const T &value_le)
                {
                        u1 buf[sizeof(T)];
                        for (size_t i = 0; i < sizeof(buf); ++i) {
                                buf[i] = *((u1 *)&value_le + sizeof(T) - i - 1);
                        }

                        this->write_bytes(buf, sizeof(buf));
                }
        };
}

#endif
