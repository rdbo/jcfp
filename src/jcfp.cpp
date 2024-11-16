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

#include <jcfp.hpp>
#include "utils.hpp"

using namespace jcfp;

#define CLASSFILE_MAGIC 0xCAFEBABE

std::expected<ClassFile, Error> ClassFile::parse(u1 *bytes)
{
	u4 magic;
	u2 minor_version;
	MajorVersion major_version;
	ConstantPool constant_pool;
	AccessFlags access_flags;
	u2 this_class;
	u2 super_class;
	std::vector<u2> interfaces;
	std::vector<u2> fields;
	std::vector<u2> methods;
	std::vector<u2> attributes;

	BufReader reader = BufReader(bytes);

	magic = reader.read_be<u4>();
	if (magic != CLASSFILE_MAGIC)
		return std::unexpected(Error { ErrorKind::WrongMagic, reader.prev_pos() });

	minor_version = reader.read_be<u2>();
	major_version = static_cast<MajorVersion>(reader.read_be<u2>());

	auto result = parse_constant_pool(reader);
	if (!result.has_value())
		return std::unexpected(result.error());
	constant_pool = result.value();

	return ClassFile(magic, minor_version, major_version, constant_pool,
	                 access_flags, this_class, super_class, interfaces,
	                 fields, methods, attributes);
}
