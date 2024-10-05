#include "syscall.hpp"

std::map< std::basic_string_view< char >, syscall::SYSCALL > syscall::syscall_table = { };

void *syscall::initialize_page( )
{
	return VirtualAlloc( 0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
}

void syscall::initialize_syscall_table( )
{
	using std::literals::string_view_literals::operator""sv;

	auto page = initialize_page( );
	if ( !page ) return;

	std::vector< std::basic_string_view< char > > ordinals =
	{
		"NtUserShowWindow"sv,
		"NtOpenProcess"sv,
		"NtUserGetAsyncKeyState"sv,
		"NtTerminateProcess"sv,
		"NtClose"sv,
		"NtAllocateVirtualMemory"sv,
		"NtFreeVirtualMemory"sv,
		"NtCreateThreadEx"sv,
		"NtWaitForSingleObject"sv,
		"NtQueryInformationThread"sv,
		"NtWriteVirtualMemory"sv,
		"NtSuspendProcess"sv
	};

	std::size_t next_view = 0;

	for ( auto &o : ordinals )
	{
		auto ntdll = LoadLibraryA( "ntdll.dll" );
		if ( !ntdll ) continue;

		auto win32u = LoadLibraryA( "win32u.dll" );
		if ( !win32u ) continue;

		auto user32 = LoadLibraryA( "user32.dll" );
		if ( !user32 ) continue;

		auto ntexport = GetProcAddress( ntdll, o.data( ) );
		if ( !ntexport )
		{
			ntexport = GetProcAddress( win32u, o.data( ) );
			if ( !ntexport ) continue;
		}

		syscall_t syscall = *( SYSCALL_ID * )( ( ULONGLONG )( ntexport ) + 4 );
		std::memcpy( PVOID( ( ULONGLONG )( page ) + next_view ), ( unsigned char * )( &syscall ), sizeof( syscall ) );

		syscall_table.insert( std::make_pair( o, SYSCALL( ( ULONGLONG )( page ) + next_view ) ) );

		next_view += sizeof( syscall );
	}
}

void syscall::initialize( )
{

	initialize_syscall_table( );
}

syscall::SYSCALL &syscall::get( std::basic_string_view< char > ordinal )
{
	return syscall_table.at( ordinal );
}