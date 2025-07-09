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

#include <jcfp/jcfp.hpp>
#include <jcfp/utils.hpp>
#include <format>

using namespace jcfp;

AttributeInfo AttributeInfo::parse(BufReader &reader)
{
	u2 attribute_name_index = reader.read_be<u2>();
	u4 attribute_length = reader.read_be<u4>();
	std::vector<u1> info = reader.read_bytes(attribute_length);
	return AttributeInfo(attribute_name_index, info);
}

AttributeInfo AttributeInfo::parse(const u1 *bytes, size_t max_length)
{
	BufReader reader = BufReader(bytes, max_length);
	return AttributeInfo::parse(reader);
}

std::vector<u1> AttributeInfo::encode()
{
	ByteStream stream = ByteStream();

	stream.write_be(this->attribute_name_index);

	u4 attribute_length = this->info.size();
	stream.write_be(attribute_length);
	stream.write_bytes(this->info);

	return stream.collect();
}
