#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <string_view>
#include <filesystem>

#include <winternl.h>

namespace process
{
	HANDLE find( std::basic_string_view< wchar_t > name );
	NTSTATUS inject( HANDLE handle, std::filesystem::path path );
	HANDLE create( std::filesystem::path path );
	void kill( HANDLE handle );
}