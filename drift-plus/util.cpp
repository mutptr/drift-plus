#include "pch.h"

#include "util.h"

uint32_t util::find_process_by_name(std::wstring_view name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 entry{};
	entry.dwSize = sizeof(entry);

	if (!Process32First(snapshot, &entry))
		return 0;

	do
	{
		if (_wcsicmp(entry.szExeFile, name.data()) == 0)
			return entry.th32ProcessID;
	} while (Process32Next(snapshot, &entry));

	CloseHandle(snapshot);

	return 0;
}

uintptr_t util::find_module_by_name(uint32_t process_id, std::wstring_view name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);
	if (snapshot == INVALID_HANDLE_VALUE)
		return 0;

	MODULEENTRY32 entry{};
	entry.dwSize = sizeof(entry);

	if (!Module32First(snapshot, &entry))
		return 0;

	do
	{
		if (_wcsicmp(entry.szModule, name.data()) == 0)
			return (uintptr_t)entry.modBaseAddr;
	} while (Module32Next(snapshot, &entry));

	CloseHandle(snapshot);

	return 0;
}