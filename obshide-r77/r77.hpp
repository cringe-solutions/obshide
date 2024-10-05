#pragma once

#include <windows.h>
#include <iostream>
#include <winternl.h>
#include <ntstatus.h>

#include "undocumented.hpp"

namespace r77
{
	typedef struct _CLIENT_ID
	{
		PVOID UniqueProcess;
		PVOID UniqueThread;
	} CLIENT_ID, *PCLIENT_ID;

	NTSTATUS NTAPI hk_nt_query_system_information(
		SYSTEM_INFORMATION_CLASS sys_info_cl,
		SystemProcessInformationEx *sys_info,
		SIZE_T sys_info_length,
		PSIZE_T rsp_length
	);
	NTSTATUS NTAPI hk_nt_open_process(
		PHANDLE handle,
		ACCESS_MASK desired_access,
		POBJECT_ATTRIBUTES object_attributes,
		PCLIENT_ID client_id
	);

	inline decltype( &hk_nt_query_system_information ) o_nt_query_system_information = nullptr;
	inline decltype( &hk_nt_open_process ) o_nt_open_process = nullptr;
}