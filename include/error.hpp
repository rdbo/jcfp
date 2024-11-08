#ifndef _JCFP_ERROR_HPP_
#define _JCFP_ERROR_HPP_

namespace jcfp {
	enum class Error {
		WrongMagic, /* The ClassFile's `magic` is wrong  */
		NotEnoughBytes, /* When `maxlength` is too short to parse something */
	};
}

#endif
