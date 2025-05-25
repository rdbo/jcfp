#ifndef _JCFP_ERROR_HPP_
#define _JCFP_ERROR_HPP_

namespace jcfp {
        enum class ErrorKind {
                Unknown,
                WrongMagic, /* The ClassFile's `magic` is wrong  */
        };

        struct Error {
                ErrorKind kind;
                size_t offset;
        };
}

#endif
