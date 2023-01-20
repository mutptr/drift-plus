#include "pch.h"

#include "blackcipher.h"

#include "offset.h"
#include "util.h"

#define PROCESS_NAME xorstr_(L"blackcipher64.aes")

#define BYPASS_ASM	"50 48 B8 22 2F 39 41 01 00 00 00 48 39 84 24 40 2F 00 00 0F 84 44 00 00 00 48 B8 4B 23 10 41 01 00 00 00 48 39 84 24 40 2F 00 00 0F 84 2C 00 00 00 48 B8 5E 86 33 41 01 00 00 00 48 39 84 24 40 2F 00 00 0F 84 14 00 00 00 58 48 31 C0 48 FF C0 48 31 D2 48 81 C4 28 2F 00 00 5F 5E C3 58 9C 52 54 5A 48 83 C2 08 48 83 EA 08 48 87 14 24 5C 48 89 3C 24 48 8B FF 53 FF 25 00 00 00 00 28 86 25 41 01 00 00 00"
#define ORIGIN_ASM	"9C 52 54 5A 48 83 C2 08 48 81 EA 08 00 00"
#define JMP_ASM		"FF 25 00 00 00 00 CC CC CC CC CC CC CC CC"

blackcipher::blackcipher()
{
	pid_ = util::find_process_by_name(PROCESS_NAME);
	PLOGI << xorstr_("NGS pid: ") << pid_;
	base_ = util::find_module_by_name(pid_, PROCESS_NAME);
	PLOGI << xorstr_("NGS base: ") << std::hex << base_;
	handle_ = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid_);
	PLOGI << xorstr_("NGS handle: ") << handle_;

	if (!handle_)
		return;

	uintptr_t hook_address = base_ + offset::ngs::hook;
	PLOGI << xorstr_("NGS hook address: ") << std::hex << hook_address;

	uint32_t checksum = 0;
	constexpr uint32_t origin_checksum = util::get_4byte_from_array<0>(util::array_from_string(BYPASS_ASM));
	constexpr uint32_t origin_checksum2 = util::get_4byte_from_array<0>(util::array_from_string(JMP_ASM));
	ReadProcessMemory(handle_, (LPCVOID)hook_address, &checksum, sizeof(checksum), nullptr);
	if (checksum == origin_checksum2)
	{
		alloc_ = (uint8_t*)1;
		return;
	}
	if (checksum != origin_checksum)
	{
		PLOGI << xorstr_("NGS 버전이 다릅니다.");
		return;
	}

	alloc_ = (uint8_t*)VirtualAllocEx(handle_, nullptr, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!alloc_)
		return;

	constexpr auto bypass_asm = util::array_from_string(BYPASS_ASM);
	*(uint64_t*)(bypass_asm.data() + 0x77 + 6) = hook_address + 0x1C;
	WriteProcessMemory(handle_, alloc_, bypass_asm.data(), bypass_asm.size(), nullptr);

	constexpr auto jmp_asm = util::array_from_string(JMP_ASM);
	*(uint64_t*)(jmp_asm.data() + 6) = (uint64_t)alloc_;
	WriteProcessMemory(handle_, (LPVOID)hook_address, jmp_asm.data(), jmp_asm.size(), nullptr);
}

blackcipher::~blackcipher()
{
	/*if (alloc_)
	{
		uintptr_t hook_address = base_ + offset::ngs::hook;
		auto origin_asm = util::array_from_string(ORIGIN_ASM);
		WriteProcessMemory(handle_, (LPVOID)hook_address, origin_asm.data(), origin_asm.size(), nullptr);
		VirtualFreeEx(handle_, alloc_, 0, MEM_RELEASE);
	}*/

	if (handle_)
		CloseHandle(handle_);
}

blackcipher::operator bool() const
{
	return alloc_;
}