#include <windows.h>
#include <algorithm>
#include <vector>
#include <string_view>
#include <tlhelp32.h>

#include "minhook/minhook.h"
#include "obs.hpp"
#include "r77.hpp"

void msgbox( )
{
	MessageBoxA( 0, "OBS successfully hidden", "obshide", MB_OK | MB_ICONINFORMATION );
}

int __stdcall DllMain( HINSTANCE instance, UINT reason, LPVOID )
{
	if ( reason == 1 )
	{
		auto snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, GetCurrentProcessId( ) );

		MODULEENTRY32 entry = { };
		entry.dwSize = sizeof( entry );

		Module32First( snapshot, &entry );

		if ( wcsstr( entry.szModule, L"obs64.exe" ) )
		{
			LoadLibraryA( "shell32.dll" );

			obs::patch_tray_icon( );

			CreateThread( 0, 0, LPTHREAD_START_ROUTINE( msgbox ), 0, 0, 0 );
			return 1;
		}

		if (
			wcsstr( entry.szModule, L"ProcessHacker.exe" ) ||
			wcsstr( entry.szModule, L"Taskmgr.exe" ) ||
			wcsstr( entry.szModule, L"perfmon.exe" )
			)
		{
			MH_Initialize( );
			MH_CreateHookApi( L"ntdll.dll", "NtQuerySystemInformation", r77::hk_nt_query_system_information, ( void ** )&r77::o_nt_query_system_information );
			MH_EnableHook( 0 );
			return 1;
		}
	}
	return 1;
}