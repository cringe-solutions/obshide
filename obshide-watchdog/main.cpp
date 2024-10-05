#include "syscall.hpp"
#include "process.hpp"
#include <iostream>

#define DLL_PATH "F:\\Programs\\MyProjects\\obshide\\x64\\Debug\\obshide-r77.dll"

HWND obs_wnd = nullptr;
DWORD obs_pid = 0;

struct
{
	bool insert_pressed;
	bool f9_pressed;

	void erase( )
	{
		this->insert_pressed = false;
		this->f9_pressed = false;
	}
} keybinds;

bool window_shown = true;

BOOL CALLBACK enum_windows_cb( HWND hwnd, LPARAM lparam )
{
	DWORD pid = 0;
	GetWindowThreadProcessId( hwnd, &pid );

	CHAR class_name[ 256 ] = { };
	GetClassNameA( hwnd, class_name, 256 );

	CHAR window_name[ 256 ] = { };
	GetWindowTextA( hwnd, window_name, 256 );

	if ( pid == obs_pid && std::string( class_name ) == "Qt663QWindowIcon" && std::string( window_name ).find( "OBS " ) != std::string::npos )
	{
		obs_wnd = hwnd;
		MessageBoxA( GetConsoleWindow( ), "Press F9+INSERT to hide/show OBS window.", "obshide", MB_OK | MB_ICONINFORMATION );
		ShowWindow( GetConsoleWindow( ), SW_HIDE );
		return FALSE;
	}

	return TRUE;
}

int main( )
{
	using syscall::operator""sys;

	printf( "This window will be closed automatically.\n" );

	syscall::initialize( );
	HANDLE process = process::find( L"obs64.exe" );
	if ( !process || process == INVALID_HANDLE_VALUE )
	{
		MessageBoxA( GetConsoleWindow( ), "OBS not found", "obshide", MB_ICONERROR | MB_OK );
		return 1;
	}

	if ( MessageBoxA( GetConsoleWindow( ), "OBS will be reloaded.\nContinue?", "obshide", MB_ICONWARNING | MB_YESNO ) != IDYES )
	{
		return 1;
	}

	auto lol = process::get_path( process ).string( );

	process::kill( process );
	process = process::create( lol );
	obs_pid = GetProcessId( process );

	process::inject( process, DLL_PATH );

	Sleep( 5000 );

	while ( !obs_wnd )
	{
		EnumWindows( enum_windows_cb, 0 );
		Sleep( 100 );
	}

	while ( true )
	{
		keybinds.f9_pressed = "NtUserGetAsyncKeyState"sys( VK_F9 );
		keybinds.insert_pressed = "NtUserGetAsyncKeyState"sys( VK_INSERT ) & ( 1 << 0 );

		if ( keybinds.insert_pressed && keybinds.f9_pressed && obs_wnd )
		{
			"NtUserShowWindow"sys( obs_wnd, window_shown ? SW_HIDE : SW_SHOW );
			window_shown = !window_shown;

			keybinds.erase( );
		}

		HANDLE process_hacker = process::find( L"ProcessHacker.exe" );
		HANDLE task_mgr = process::find( L"Taskmgr.exe" );
		HANDLE perfmon = process::find( L"perfmon.exe" );

		process::inject( process_hacker, DLL_PATH );
		process::inject( task_mgr, DLL_PATH );
		process::inject( perfmon, DLL_PATH );

		Sleep( 10 );
	}

	return 0;
}