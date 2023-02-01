#pragma once

#include "engine.h"

class mainform : public nana::form
{
public:
	mainform(engine* eng);
	~mainform();
	
private:
	engine* eng = nullptr;
	nana::checkbox booster_{ *this };
	nana::textbox booster_value_{ *this };

	nana::checkbox speed_{ *this };
	nana::checkbox speed_space_{ *this };
	nana::textbox speed_value_{ *this };

	nana::checkbox crash_guard_{ *this };
	nana::checkbox team_booster_{ *this };
	nana::checkbox smooth_drift_{ *this };
	nana::checkbox body_{ *this };


	constexpr static float default_value_ = 1.0f;
	bool running_ = true;
	std::thread speed_t_;
};