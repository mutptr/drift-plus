#include "pch.h"

#include "mainform.h"

#include "engine.h"

mainform::mainform(engine* eng)
	: nana::form{ nana::API::make_center(400, 300), appear::decorate<appear::minimize>{} },
	eng{ eng }
{
	caption(xorstr_("drift-plus"));

	auto booster_value_click = [&]
	{
		if (booster_.checked())
		{
			float val = (float)atof(booster_value_.text().data());
			if (val)
				eng->booster(val);
		}
	};

	booster_.move({ 20, 20, 160, 25 });
	booster_.caption(xorstr_("부스터 조절"));
	auto booster_click = [&, booster_value_click]
	{
		if (booster_.checked())
		{
			booster_value_click();
		}
		else
		{
			eng->booster(default_value_);
		}
	};
	booster_.events().click(booster_click);

	booster_value_.move({ 20, 50, 360, 25 });
	booster_value_.caption(xorstr_("1.0"));
	booster_value_.multi_lines(false);
	booster_value_.events().text_changed(booster_value_click);


	speed_.move({ 20, 100, 160, 25 });
	speed_.caption(xorstr_("스피드 조절"));
	speed_space_.move({ 200, 100, 160, 25 });
	speed_space_.caption(xorstr_("Space"));

	speed_value_.move({ 20, 130, 360, 25 });
	speed_value_.caption(xorstr_("1.0"));
	speed_value_.multi_lines(false);

	speed_t_ = std::thread{ [&]
	{
		while (running_)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			if (speed_.checked())
			{
				float val = (float)atof(speed_value_.text().data());
				if (val)
				{
					if (!speed_space_.checked())
					{
						eng->speed(val);
					}
					else if (GetAsyncKeyState(VK_SPACE) & 0x8000)
					{
						eng->speed(val);
					}
					else
					{
						eng->speed(default_value_);
					}
				}
			}
			else
			{
				eng->speed(default_value_);
			}
		}
	} };

	crash_guard_.move({ 20, 180, 160, 25 });
	crash_guard_.caption(xorstr_("부스터 손실 제거"));
	auto crash_guard_click = [&]
	{
		crash_guard_.checked() ?
			eng->enable_crash_guard() :
			eng->disable_crash_guard();
	};
	crash_guard_.events().click(crash_guard_click);

	team_booster_.move({ 200, 180, 160, 25 });
	team_booster_.caption(xorstr_("팀부 무한"));
	auto team_booster_click = [&]
	{
		team_booster_.checked() ?
			eng->enable_team_booster() :
			eng->disable_team_booster();
	};
	team_booster_.events().click(team_booster_click);
}

mainform::~mainform()
{
	running_ = false;
	if (speed_t_.joinable())
		speed_t_.join();
}