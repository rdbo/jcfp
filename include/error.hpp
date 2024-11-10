#ifndef _JCFP_ERROR_HPP_
#define _JCFP_ERROR_HPP_

namespace jcfp {
	enum class Error {
		Unknown,
		WrongMagic, /* The ClassFile's `magic` is wrong  */
	};
}

#endif
