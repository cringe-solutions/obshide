#include "process.hpp"
#include "syscall.hpp"

#include <ntstatus.h>

HANDLE process::find( std::basic_string_view< wchar_t > name )
{
	using syscall::operator""sys;

	HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
	if ( !snapshot ) return nullptr;

	PROCESSENTRY32 entry = { };
	entry.dwSize = sizeof( entry );
	if ( Process32First( snapshot, &entry ) )
	{
		do
		{
			if ( wcsstr( entry.szExeFile, name.data( ) ) )
			{
				HANDLE handle = nullptr;
				OBJECT_ATTRIBUTES oa = { };
				InitializeObjectAttributes( &oa, 0, 0, 0, 0 );
				CLIENT_ID id = { };
				id.UniqueProcess = HANDLE( entry.th32ProcessID );

				"NtClose"sys( snapshot );
				"NtOpenProcess"sys( &handle, PROCESS_ALL_ACCESS, &oa, &id );
				return handle;
			}
		} while ( Process32Next( snapshot, &entry ) );
	}

	"NtClose"sys( snapshot );

	return nullptr;
}

NTSTATUS process::inject( HANDLE handle, std::filesystem::path path )
{
	using syscall::operator""sys;

	if ( !handle || handle == INVALID_HANDLE_VALUE ) return STATUS_ASSERTION_FAILURE;
	if ( !std::filesystem::exists( path ) ) return STATUS_NOT_FOUND;

	auto s_path = path.string( );

	PVOID base_address = nullptr;
	SIZE_T region_size = 0x1000;

	auto status = "NtAllocateVirtualMemory"sys( handle, &base_address, ULONG_PTR( 0 ), &region_size, ULONG( MEM_COMMIT | MEM_RESERVE ), ULONG( PAGE_EXECUTE_READWRITE ) );
	if ( !NT_SUCCESS( status ) ) return status;

	SIZE_T written = 0;
	status = "NtWriteVirtualMemory"sys( handle, base_address, s_path.c_str( ), SIZE_T( s_path.length( ) + 1 ), &written );
	if ( !NT_SUCCESS( status ) ) return status;

	auto kernel32 = GetModuleHandleA( "kernel32.dll" );
	if ( !kernel32 ) return STATUS_DLL_NOT_FOUND;

	auto loadlibrary = GetProcAddress( kernel32, "LoadLibraryA" );
	if ( !loadlibrary ) return STATUS_PROCEDURE_NOT_FOUND;

	HANDLE thread_handle = nullptr;
	OBJECT_ATTRIBUTES oa = { };
	InitializeObjectAttributes( &oa, 0, 0, 0, 0 );
	status = "NtCreateThreadEx"sys( &thread_handle, ACCESS_MASK( 0x1FFFFF ), &oa, handle, loadlibrary, base_address, ULONG( 0 ), SIZE_T( 0 ), SIZE_T( 0 ), SIZE_T( 0 ), PVOID( 0 ) );
	if ( !NT_SUCCESS( status ) ) return status;
	
	LARGE_INTEGER timeout = { };
	timeout.QuadPart = INFINITE;
	status = "NtWaitForSingleObject"sys( thread_handle, BOOLEAN( FALSE ), &timeout );
	if ( !NT_SUCCESS( status ) ) return status;

	return STATUS_SUCCESS;
}

std::filesystem::path process::get_path( HANDLE handle )
{
	using syscall::operator""sys;

	auto snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, GetProcessId( handle ) );
	
	MODULEENTRY32 entry = { };
	entry.dwSize = sizeof( entry );

	Module32First( snapshot, &entry );

	"NtClose"sys( snapshot );
	return entry.szExePath;
}

HANDLE process::create( std::filesystem::path path )
{
	if ( !std::filesystem::exists( path ) ) return INVALID_HANDLE_VALUE;

	auto s_path = path.string( );
	auto dir = path.parent_path( ).string( );

	STARTUPINFOA si = { };
	PROCESS_INFORMATION pi = { };

	if ( !CreateProcessA( s_path.c_str( ), LPSTR( s_path.c_str( ) ), 0, 0, 0, 0, 0, dir.c_str( ), &si, &pi ) ) return INVALID_HANDLE_VALUE;

	return pi.hProcess;
}

void process::kill( HANDLE handle )
{
	TerminateProcess( handle, 0 );
}