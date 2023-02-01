#pragma once

class engine
{
public:
	engine();
	~engine();

	void enable_crash_guard();
	void disable_crash_guard();

	void enable_smooth_drift();
	void disable_smooth_drift();

	void enable_team_booster();
	void disable_team_booster();

	void enable_body();
	void disable_body();

	void booster(float value);
	void speed(float value);

private:
	uint32_t pid_ = 0;
	HANDLE handle_ = nullptr;

	uint64_t client_ = 0;
	uint64_t booster_alloc_ = 0;
	uint64_t booster_value_alloc_ = 0;
	uint64_t team_booster_alloc_ = 0;
	uint64_t team_booster_value_alloc_ = 0;

	uint64_t physx3_ = 0;
	uint64_t speed_alloc_ = 0;
	uint64_t speed_value_alloc_ = 0;

	void hook_booster();
	void unhook_booster();

	void hook_speed();
	void unhook_speed();

	void hook_team_booster();
	void unhook_team_booster();
};