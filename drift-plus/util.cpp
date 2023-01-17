#include "pch.h"

#include "util.h"

uint32_t util::find_process_by_name(std::wstring_view name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot)
	{
		PROCESSENTRY32 entry{};

		if (Process32First(snapshot, &entry))
		{
			do
			{
				if (_wcsicmp(entry.szExeFile, name.data()) == 0)
					return entry.th32ProcessID;
			} while (Process32Next(snapshot, &entry));
		}

		CloseHandle(snapshot);
	}

	return 0;
}