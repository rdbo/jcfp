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

std::expected<ConstantPoolEntry, Error> ConstantPoolEntry::parse(u1 *bytes)
{
	BufReader reader = BufReader(bytes);
	u1 tag = reader.read<u1>();
	EntryVariant info;

	switch (tag) {
	case Tag::CONSTANT_Class:
		break;
	case Tag::CONSTANT_Fieldref:
		break;
	case Tag::CONSTANT_Methodref:
		break;
	case Tag::CONSTANT_InterfaceMethodref:
		break;
	case Tag::CONSTANT_String:
		break;
	case Tag::CONSTANT_Integer:
		break;
	case Tag::CONSTANT_Float:
		break;
	case Tag::CONSTANT_Long:
		break;
	case Tag::CONSTANT_Double:
		break;
	case Tag::CONSTANT_NameAndType:
		break;
	case Tag::CONSTANT_Utf8:
		break;
	case Tag::CONSTANT_MethodHandle:
		break;
	case Tag::CONSTANT_MethodType:
		break;
	case Tag::CONSTANT_InvokeDynamic:
		break;
	case Tag::Empty:
		break;
	}

	return ConstantPoolEntry();
}

std::expected<ConstantPool, Error> ConstantPool::parse(u1 *bytes)
{
	std::vector<ConstantPoolEntry> entries;

	BufReader reader = BufReader(bytes);

	u2 constant_pool_count = reader.read_be<u2>();

	if (constant_pool_count == 0)
		return ConstantPool(entries);

	// The first entry is always an empty tag
	entries.push_back(ConstantPoolEntry());

	for (u2 i = 1; i < constant_pool_count - 1; ++i) {
		
	}

	return ConstantPool(entries);
}
