/*
 * Copyright (C) 2020 Daniel Machon
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <windows.h>
#include <psapi.h>

#include <stdio.h>

DWORD_PTR proc_get_base_address(DWORD processID)
{
	DWORD_PTR baseAddress = 0;
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	HMODULE *moduleArray;
	LPBYTE moduleArrayBytes;
	DWORD bytesRequired;

	if (processHandle) {
		if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired)) {
			if (bytesRequired) {
				moduleArrayBytes = (LPBYTE) LocalAlloc(LPTR, bytesRequired);

				if (moduleArrayBytes) {
					unsigned int moduleCount;

					moduleCount = bytesRequired / sizeof( HMODULE);
					moduleArray = (HMODULE *) moduleArrayBytes;

					if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired)) {
						baseAddress = (DWORD_PTR) moduleArray[0];
					}

					LocalFree(moduleArrayBytes);
				}
			}
		}

		CloseHandle(processHandle);
	}

	return baseAddress;
}

int proc_print_id_and_name(DWORD processID, char *name)
{

	char szProcessName[MAX_PATH];
	HANDLE hProcess = NULL;
	char buff[200] = "";

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	/* Get the process name. */
	if (NULL != hProcess) {
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleBaseNameA(hProcess, hMod, (LPSTR) szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
		}
	}

	if (lstrcmpiA(szProcessName, name) == 0) {
		CloseHandle(hProcess);
		return 1;
	}

	return 0;

	CloseHandle(hProcess);
}

int proc_get_id(char *name, PDWORD pid)
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	UINT32 i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		//Error 
	}

	/* Calculate how many process identifiers were returned. */
	cProcesses = cbNeeded / sizeof(DWORD);

	/* Print the name and process identifier for each process. */
	for (i = 0; i < cProcesses; i++) {
		if (aProcesses[i] != 0) {
			if (proc_print_id_and_name(aProcesses[i], name) == 1) {
				*pid = aProcesses[i];
				break;
			}
		}
	}
	return 0;
}