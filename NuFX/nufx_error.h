#ifndef __nufx_error__
#define __nufx_error__


#include <system_error>
#include <NufxLib.h>

	std::error_category &nufx_category();

namespace std {

	template<>
	struct is_error_code_enum<NuError> : public true_type {};

	template<>
	struct is_error_condition_enum<NuError> : public true_type {};

}

// hmm... this should not be in a namespace.
inline std::error_condition make_error_condition(NuError e)
{
	// positive values are posix errors.
	return e < 0
		? std::error_condition(e, nufx_category())
		: std::error_condition(e, std::generic_category())
		;
}


inline std::error_code make_error_code(NuError e)
{
	// positive values are posix errors.
	return e < 0
		? std::error_code(e, nufx_category())
		: std::error_code(e, std::generic_category())
		;
}


#endif
