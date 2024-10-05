#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <string_view>
#include <filesystem>

namespace process
{
	HANDLE find( std::basic_string_view< wchar_t > name );
	NTSTATUS inject( HANDLE handle, std::filesystem::path path );
	std::filesystem::path get_path( HANDLE handle );
	HANDLE create( std::filesystem::path path );
	void kill( HANDLE handle );
}