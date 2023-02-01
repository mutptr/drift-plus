#pragma once

class blackcipher
{
public:
	blackcipher();
	~blackcipher();

	operator bool() const;

private:
	HANDLE handle_ = nullptr;
	uint32_t pid_ = 0;
	uintptr_t base_ = 0;
	uint8_t* alloc_ = 0;

	template<size_t N>
	void crc(uint32_t offset, std::array<uint8_t, N>&& arr);
};