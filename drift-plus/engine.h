#pragma once

class engine
{
public:
	engine();
	~engine();

	void booster(float value);
	void speed(float value);

private:
	uint32_t pid_ = 0;
	HANDLE handle_ = nullptr;

	uint64_t client_ = 0;
	uint64_t booster_alloc_ = 0;
	uint64_t booster_value_alloc_ = 0;

	uint64_t physx3_ = 0;
	uint64_t speed_alloc_ = 0;
	uint64_t speed_value_alloc_ = 0;
};