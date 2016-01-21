#include "nufx_error.h"
#include <string>

namespace {
	class private_error_category : public std::error_category {

		virtual std::error_condition default_error_condition( int code ) const noexcept;
		virtual std::string message( int condition ) const noexcept;
		virtual const char* name() const noexcept;
	};

	std::error_condition private_error_category::default_error_condition( int code ) const noexcept
	{

		switch(code)
		{
		case kNuErrInvalidArg:
			return std::errc::invalid_argument;
		case kNuErrMalloc:
			return std::errc::not_enough_memory;
		case kNuErrFileNotFound:
			return std::errc::no_such_file_or_directory;
		case kNuErrNotDir:
			return std::errc::not_a_directory;
		case kNuErrFileAccessDenied:
			return std::errc::permission_denied;
			
		default:
			return std::error_condition(code, *this);
		}
	}

	const char* private_error_category::name() const noexcept
	{
		return "NuFX Error";
	}
	std::string private_error_category::message( int condition ) const noexcept
	{
		return NuStrError((NuError)condition);
	}

}

std::error_category &nufx_category()
{
	static private_error_category ec;
	return ec;	
}
