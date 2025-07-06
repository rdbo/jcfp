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

#ifndef _JCFP_CONSTANT_POOL_HPP_
#define _JCFP_CONSTANT_POOL_HPP_

#include <vector>
#include <variant>
#include <string>
#include "utils.hpp"
#include "basetypes.hpp"
#include "error.hpp"

namespace jcfp {
        class ConstantPoolEntry {
        public:
                enum Tag : u1 {
                        /*
                         * Empty entries are used for:
                         *   - The first index of the constant pool, which is always invalid
                         *   - The additional unused indices of Double and Long entries
                         */
                        Empty = 0,

                        /* Standard constant pool entries */
                        Class              = 7,
                        Fieldref           = 9,
                        Methodref          = 10,
                        InterfaceMethodref = 11,
                        String             = 8,
                        Integer            = 3,
                        Float              = 4,
                        Long               = 5,
                        Double             = 6,
                        NameAndType        = 12,
                        Utf8               = 1,
                        MethodHandle       = 15,
                        MethodType         = 16,
                        InvokeDynamic      = 18,
                };

                typedef struct {} EmptyInfo;

                typedef struct {
                        u2 name_index;
                } ClassInfo;

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
                } FieldrefInfo;

                typedef struct {
                        u2 class_index;
                        u2 name_and_type_index;
                } MethodrefInfo;

                typedef struct {
                        u2 class_index;
                        u2 name_and_type_index;
                } InterfaceMethodrefInfo;

                typedef struct {
                        u2 string_index;
                } StringInfo;

                typedef struct {
                        u4 bytes;
                } IntegerInfo;

                typedef struct {
                        u4 bytes;
                } FloatInfo;

                typedef struct {
                        u4 high_bytes;
                        u4 low_bytes;
                } LongInfo;

                typedef struct {
                        u4 high_bytes;
                        u4 low_bytes;
                } DoubleInfo;

                typedef struct {
                        u2 name_index;
                        u2 descriptor_index;
                } NameAndTypeInfo;

                typedef struct {
                        // u2 length;
                        // u1 bytes[];
                        std::string bytes;
                } Utf8Info;

                typedef struct {
                        u1 reference_kind;
                        u2 reference_index;
                } MethodHandleInfo;

                typedef struct {
                        u2 descriptor_index;
                } MethodTypeInfo;

                typedef struct {
                        u2 bootstrap_method_attr_index;
                        u2 name_and_type_index;
                } InvokeDynamicInfo;
        public:
                using EntryVariant = std::variant<
                        EmptyInfo,

                        ClassInfo,

                        FieldrefInfo,
                        MethodrefInfo,
                        InterfaceMethodrefInfo,

                        StringInfo,
                        IntegerInfo,
                        FloatInfo,

                        LongInfo,
                        DoubleInfo,

                        NameAndTypeInfo,
                        Utf8Info,
                        MethodHandleInfo,
                        MethodTypeInfo,
                        InvokeDynamicInfo
                >;
                Tag tag;
                EntryVariant info;
        public:
                ConstantPoolEntry() : tag(Tag::Empty) {}
                ConstantPoolEntry(EntryVariant info) : info(info) {
                        Tag tag_table[] = {
                                Tag::Empty,

                                Tag::Class,

                                Tag::Fieldref,
                                Tag::Methodref,
                                Tag::InterfaceMethodref,

                                Tag::String,
                                Tag::Integer,
                                Tag::Float,

                                Tag::Long,
                                Tag::Double,

                                Tag::NameAndType,
                                Tag::Utf8,
                                Tag::MethodHandle,
                                Tag::InvokeDynamic
                        };

                        this->tag = tag_table[info.index()];
                }

                ConstantPoolEntry(ClassInfo info) : tag(Tag::Class), info(info) {}

                ConstantPoolEntry(FieldrefInfo info) : tag(Tag::Fieldref), info(info) {}
                ConstantPoolEntry(MethodrefInfo info) : tag(Tag::Methodref), info(info) {}
                ConstantPoolEntry(InterfaceMethodrefInfo info) : tag(Tag::InterfaceMethodref), info(info) {}

                ConstantPoolEntry(StringInfo info) : tag(Tag::String), info(info) {}
                ConstantPoolEntry(IntegerInfo info) : tag(Tag::Integer), info(info) {}
                ConstantPoolEntry(FloatInfo info) : tag(Tag::Float), info(info) {}

                ConstantPoolEntry(LongInfo info) : tag(Tag::Long), info(info) {}
                ConstantPoolEntry(DoubleInfo info) : tag(Tag::Double), info(info) {}

                ConstantPoolEntry(NameAndTypeInfo info) : tag(Tag::NameAndType), info(info) {}
                ConstantPoolEntry(Utf8Info info) : tag(Tag::Utf8), info(info) {}
                ConstantPoolEntry(MethodHandleInfo info) : tag(Tag::MethodHandle), info(info) {}
                ConstantPoolEntry(InvokeDynamicInfo info) : tag(Tag::InvokeDynamic), info(info) {}
        public:
                static std::expected<ConstantPoolEntry, Error> parse(BufReader &reader);
                static std::expected<ConstantPoolEntry, Error> parse(const u1 *bytes, size_t max_length=0);
                static inline std::expected<ConstantPoolEntry, Error> parse(const std::vector<u1> &bytes) { return parse(bytes.data(), bytes.size()); }
                std::vector<u1> encode();
                std::string to_string();

                template <typename T>
                inline T &get()
                {
                        return std::get<T>(this->info);
                }

                inline bool is_wide_entry()
                {
                        return tag == Tag::Double || tag == Tag::Long;
                }
                void relocate(int diff, u2 from);
        };

        class ConstantPool {
        private:
                /* Modifying the entries directly could cause issues, use the helper functions */
                std::vector<ConstantPoolEntry> entries;
        public:
                ConstantPool() {}
                ConstantPool(std::vector<ConstantPoolEntry> entries) : entries(entries) {}
        public:
                static std::expected<ConstantPool, Error> parse(BufReader &reader);
                static std::expected<ConstantPool, Error> parse(const u1 *bytes, size_t max_length=0);
                static inline std::expected<ConstantPool, Error> parse(const std::vector<u1> &bytes) { return parse(bytes.data(), bytes.size()); }
                std::vector<u1> encode();
        public:
                /*
                 * The constant pool entries are defined as:
                 *
                 *     cp_info constant_pool[constant_pool_count-1];
                 *
                 * Iteration goes from 1 to count - 1
                 */
                inline u2 count() {
                        return entries.size();
                }

                /* Helper functions */
                inline ConstantPoolEntry::Tag get_tag(u2 index) {
                        return entries[index].tag;
                }

                inline ConstantPoolEntry &get_entry(u2 index) {
                        return entries[index];
                }

                inline std::vector<ConstantPoolEntry> &get_entries() {
                        return this->entries;
                }

                template <typename T>
                inline T &get(u2 index)
                {
                        return entries[index].get<T>();
                }

                inline void push_entry(ConstantPoolEntry entry) {
                        entries.push_back(entry);
                        if (entry.is_wide_entry()) {
                                entries.push_back(ConstantPoolEntry());
                        }
                }

                inline ConstantPoolEntry pop_entry() {
                        if (entries.size() > 1 && entries[entries.size() - 2].is_wide_entry()) {
                                entries.pop_back(); // Last entry is Empty added due to a preceding wide entry,
                                                    // will be skipped
                        }

                        ConstantPoolEntry last_entry = entries.back();
                        entries.pop_back();

                        return last_entry;
                }

                inline void insert_entry(u2 index, ConstantPoolEntry entry)
                {
                        // Relocate references to indices >= index
                        // int diff = entry.is_wide_entry() ? +2 : +1;
                        // this->relocate(diff, index);

                        // Insert entry
                        if (entry.is_wide_entry())
                                entries.insert(entries.begin() + index, ConstantPoolEntry());
                        entries.insert(entries.begin() + index, entry);
                }

                // WARN: If anyone is referencing the entry you removed,
                //       you will get a bad relocation. Use 'replace_entry'
                //       with the same entry type if you wanna avoid such issues.
                inline ConstantPoolEntry remove_entry(u2 index)
                {
                        if (index > 1 && entries[index - 1].is_wide_entry())
                                --index;

                        // Relocate references to indices >= index
                        ConstantPoolEntry entry = entries[index];
                        // int diff = entry.is_wide_entry() ? -2 : -1;
                        // this->relocate(diff, index);

                        // Remove entry
                        entries.erase(entries.begin() + index);
                        if (entry.is_wide_entry()) {
                                entries.erase(entries.begin() + index);
                        }

                        return entry;
                }


                inline ConstantPoolEntry replace_entry(u2 index, ConstantPoolEntry entry)
                {
                        auto old_entry = entries[index];
                        entries[index] = entry;
                        if (entry.is_wide_entry() && !old_entry.is_wide_entry()) {
                                this->insert_entry(index + 1, ConstantPoolEntry());
                                this->relocate(+1, index + 1);
                        } else if (old_entry.is_wide_entry()) {
                                this->remove_entry(index + 1); // Remove empty pool entry
                                this->relocate(-1, index + 1);
                        }

                        return old_entry;
                }

                void relocate(int diff, u2 from);
        };
}

#endif
