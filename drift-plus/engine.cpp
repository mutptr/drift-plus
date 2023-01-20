#include "pch.h"

#include "engine.h"

#include "util.h"
#include "offset.h"

#define PROCESS_NAME xorstr_(L"KartDrift-Win64-Shipping.exe")

#define BOOSTER_ASM			"F3 0F 59 15 F8 04 00 00 F3 0F 5E C1 F3 0F 59 C2 F3 0F 58 C3 F3 0F 11 41 1C C3"
#define BOOSTER_ORIGIN_ASM	"F3 0F 5E C1 F3 0F 59 C2 F3 0F 58 C3 F3 0F"

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
	constexpr uint32_t origin_checksum = util::get_4byte_from_array<0>(util::array_from_string(BOOSTER_ORIGIN_ASM));
	constexpr uint32_t origin_checksum2 = util::get_4byte_from_array<0>(util::array_from_string(JMP_ASM));
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

		constexpr auto booster_asm = util::array_from_string(BOOSTER_ASM);
		WriteProcessMemory(handle_, (LPVOID)(booster_alloc_), booster_asm.data(), booster_asm.size(), nullptr);

		constexpr auto jmp_asm = util::array_from_string(JMP_ASM);
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

		constexpr auto speed_asm = util::array_from_string(SPEED_ASM);
		*(uint64_t*)(speed_asm.data() + 0x17 + 6) = physx3_ + offset::client::physx3::speed + 0xF;
		WriteProcessMemory(handle_, (LPVOID)(speed_alloc_), speed_asm.data(), speed_asm.size(), nullptr);

		constexpr auto jmp_asm = util::array_from_string(JMP_ASM);
		*(uint64_t*)(jmp_asm.data() + 6) = speed_alloc_;
		WriteProcessMemory(handle_, (LPVOID)(physx3_ + offset::client::physx3::speed), jmp_asm.data(), jmp_asm.size(), nullptr);
	}
}

engine::~engine()
{
	disable_crash_guard();
	booster(1.0f);
	speed(1.0f);

	if (booster_alloc_)
	{
		constexpr auto origin_asm = util::array_from_string(BOOSTER_ORIGIN_ASM);
		WriteProcessMemory(handle_, (LPVOID)(client_ + offset::client::booster), origin_asm.data(), origin_asm.size(), nullptr);
		VirtualFreeEx(handle_, (LPVOID)booster_alloc_, 0, MEM_RELEASE);
	}

	if (speed_alloc_)
	{
		constexpr auto origin_asm = util::array_from_string(SPEED_ORIGIN_ASM);
		WriteProcessMemory(handle_, (LPVOID)(physx3_ + offset::client::physx3::speed), origin_asm.data(), origin_asm.size(), nullptr);
		VirtualFreeEx(handle_, (LPVOID)speed_alloc_, 0, MEM_RELEASE);
	}

	

	if (handle_)
		CloseHandle(handle_);
}

void engine::enable_crash_guard()
{
	constexpr auto arr = util::array_from_string("90 90 90 90 90");
	WriteProcessMemory(handle_, (LPVOID)(client_ + offset::client::crash_guard), arr.data(), arr.size(), nullptr);
}

void engine::disable_crash_guard()
{
	constexpr auto arr = util::array_from_string("F3 0F 11 43 1C");
	WriteProcessMemory(handle_, (LPVOID)(client_ + offset::client::crash_guard), arr.data(), arr.size(), nullptr);
}

void engine::booster(float value)
{
	constexpr float booster_1_default = 3.0f;
	constexpr float booster_2_default = 1.5f;

	WriteProcessMemory(handle_, (LPVOID)booster_value_alloc_, &value, sizeof(value), nullptr);

	const float booster_1 = booster_1_default * value;
	WriteProcessMemory(handle_, (LPVOID)(client_ + offset::client::booster_value_1), &booster_1, sizeof(booster_1), nullptr);

	const float booster_2 = booster_2_default * value;
	WriteProcessMemory(handle_, (LPVOID)(client_ + offset::client::booster_value_2), &booster_2, sizeof(booster_2), nullptr);
}

void engine::speed(float value)
{
	WriteProcessMemory(handle_, (LPVOID)speed_value_alloc_, &value, sizeof(value), nullptr);
}