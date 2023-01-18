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
};