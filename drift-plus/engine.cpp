#include "pch.h"

#include "engine.h"

#include "util.h"
#include "offset.h"

#define PROCESS_NAME xorstr_(L"KartDrift-Win64-Shipping.exe")

#define BOOSTER_ASM			"F3 0F 59 05 F8 04 00 00 89 4B 18 89 43 20 F3 0F 11 43 10 89 4B 1C FF 25 00 00 00 00 6C 41 40 41 01 00 00 00"
#define BOOSTER_ORIGIN_ASM	"89 4B 18 89 43 20 F3 0F 11 43 10 89 4B 1C"

#define SPEED_ASM			"F3 0F 59 05 F8 04 00 00 F3 0F 59 F0 0F 28 FA F3 0F 59 F8 44 0F 28 C3 FF 25 00 00 00 00 F5 3F D7 11 FE 7F 00 00"
#define SPEED_ORIGIN_ASM	"F3 0F 59 F0 0F 28 FA F3 0F 59 F8 44 0F 28"

#define JMP_ASM				"FF 25 00 00 00 00 CC CC CC CC CC CC CC CC"

engine::engine()
{
	pid_ = util::find_process_by_name(PROCESS_NAME);
	PLOGI << xorstr_("Client pid: ") << pid_;

	handle_ = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid_);
	PLOGI << xorstr_("Client handle: ") << handle_;

	if (!handle_)
		return;

	client_ = util::find_module_by_name(pid_, PROCESS_NAME);
	physx3_ = util::find_module_by_name(pid_, xorstr_(L"PhysX3_x64.dll"));

	PLOGI << xorstr_("Client base: ") << std::hex << client_;
	PLOGI << xorstr_("PhysX3 base: ") << std::hex << physx3_;

	uint32_t checksum = 0;
	uint32_t origin_checksum = *(uint32_t*)(util::array_from_string(BOOSTER_ORIGIN_ASM).data());
	uint32_t origin_checksum2 = *(uint32_t*)(util::array_from_string(JMP_ASM).data());
	ReadProcessMemory(handle_, (LPCVOID)(client_ + offset::client::booster), &checksum, sizeof(checksum), nullptr);
	if (checksum != origin_checksum && checksum != origin_checksum2)
	{
		PLOGI << xorstr_(L"클라이언트 버전이 다릅니다.");
		return;
	}

	booster_alloc_ = (uint64_t)VirtualAllocEx(handle_, nullptr, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	PLOGI << xorstr_("Booster alloc: ") << std::hex << booster_alloc_;
	if (client_ && booster_alloc_)
	{
		booster_value_alloc_ = booster_alloc_ + 0x500;
		constexpr float default_value = 1.0f;
		WriteProcessMemory(handle_, (LPVOID)booster_value_alloc_, &default_value, sizeof(default_value), nullptr);

		auto booster_asm = util::array_from_string(BOOSTER_ASM);
		*(uint64_t*)(booster_asm.data() + 0x16 + 6) = client_ + offset::client::booster + 0xE;
		WriteProcessMemory(handle_, (LPVOID)(booster_alloc_), booster_asm.data(), booster_asm.size(), nullptr);

		auto jmp_asm = util::array_from_string(JMP_ASM);
		*(uint64_t*)(jmp_asm.data() + 6) = booster_alloc_;
		WriteProcessMemory(handle_, (LPVOID)(client_ + offset::client::booster), jmp_asm.data(), jmp_asm.size(), nullptr);
	}

	speed_alloc_ = (uint64_t)VirtualAllocEx(handle_, nullptr, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	PLOGI << xorstr_("Speed alloc: ") << std::hex << speed_alloc_;
	if (physx3_ && speed_alloc_)
	{
		speed_value_alloc_ = speed_alloc_ + 0x500;
		constexpr float default_value = 1.0f;
		WriteProcessMemory(handle_, (LPVOID)speed_value_alloc_, &default_value, sizeof(default_value), nullptr);

		auto speed_asm = util::array_from_string(SPEED_ASM);
		*(uint64_t*)(speed_asm.data() + 0x17 + 6) = physx3_ + offset::client::physx3::speed + 0xF;
		WriteProcessMemory(handle_, (LPVOID)(speed_alloc_), speed_asm.data(), speed_asm.size(), nullptr);

		auto jmp_asm = util::array_from_string(JMP_ASM);
		*(uint64_t*)(jmp_asm.data() + 6) = speed_alloc_;
		WriteProcessMemory(handle_, (LPVOID)(physx3_ + offset::client::physx3::speed), jmp_asm.data(), jmp_asm.size(), nullptr);
	}
}

engine::~engine()
{
	if (booster_alloc_)
	{
		auto origin_asm = util::array_from_string(BOOSTER_ORIGIN_ASM);
		WriteProcessMemory(handle_, (LPVOID)(client_ + offset::client::booster), origin_asm.data(), origin_asm.size(), nullptr);
		VirtualFreeEx(handle_, (LPVOID)booster_alloc_, 0, MEM_RELEASE);
	}

	if (speed_alloc_)
	{
		auto origin_asm = util::array_from_string(SPEED_ORIGIN_ASM);
		WriteProcessMemory(handle_, (LPVOID)(physx3_ + offset::client::physx3::speed), origin_asm.data(), origin_asm.size(), nullptr);
		VirtualFreeEx(handle_, (LPVOID)speed_alloc_, 0, MEM_RELEASE);
	}

	if (handle_)
		CloseHandle(handle_);
}

void engine::booster(float value)
{
	WriteProcessMemory(handle_, (LPVOID)booster_value_alloc_, &value, sizeof(value), nullptr);
}

void engine::speed(float value)
{
	WriteProcessMemory(handle_, (LPVOID)speed_value_alloc_, &value, sizeof(value), nullptr);
}