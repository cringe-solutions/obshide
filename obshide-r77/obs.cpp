#include "obs.hpp"

bool obs::patch_tray_icon( )
{
	auto shell32 = GetModuleHandleA( "shell32.dll" );
	while ( !shell32 )
	{
		shell32 = GetModuleHandleA( "shell32.dll" );
		Sleep( 100 );
	}
	
	auto shell_notifyiconw = GetProcAddress( shell32, "Shell_NotifyIconW" );
	if ( !shell_notifyiconw ) return false;

	DWORD old = 0;
	auto status = VirtualProtect( shell_notifyiconw, 0x1, PAGE_EXECUTE_READWRITE, &old );
	if ( !status ) return false;

	*( std::uint8_t * )( shell_notifyiconw ) = 0xC3; // RET

	status = VirtualProtect( shell_notifyiconw, 0x1, old, &old );
	if ( !status ) return false;

	return true;
}