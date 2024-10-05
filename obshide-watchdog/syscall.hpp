#pragma once

#include <windows.h>
#include <winternl.h>
#include <iostream>
#include <functional>
#include <string_view>
#include <vector>
#include <map>

namespace syscall
{
	template < unsigned __int64 size > using ASM = const unsigned char[ size ];
	using SYSCALL = NTSTATUS( __stdcall * )( ... );
	using SYSCALL_ID = unsigned;

#pragma pack( push, 1 )
	struct syscall_t
	{
		private:
		ASM< 3 > asm_0x0 = { 0x4C, 0x8B, 0xD1 }; // MOV R10, RCX
		ASM< 1 > asm_0x3 = { 0xB8 }; // MOV EAX, ????????
		public:
		SYSCALL_ID syscall_index;
		private:
		ASM< 2 > asm_0x8 = { 0x0F, 0x05 }; // SYSCALL
		ASM< 1 > asm_0xA = { 0xC3 }; // RET

		public:
		syscall_t( SYSCALL_ID idx ) : syscall_index( idx )
		{

		}
	};
#pragma pack( pop )

	void *initialize_page( );
	void initialize_syscall_table( );

	void initialize( );

	SYSCALL &get( std::basic_string_view< char > ordinal );

	inline SYSCALL &operator ""sys( const char *ordinal, size_t )
	{
		return get( ordinal );
	}

	extern std::map< std::basic_string_view< char >, SYSCALL > syscall_table;
}