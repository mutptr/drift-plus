#include "pch.h"

#include "mainform.h"

#include "engine.h"

mainform::mainform(engine* eng)
	: nana::form{ nana::API::make_center(400, 300), appear::decorate<appear::minimize>{} },
	eng{ eng }
{
	booster_.move({ 20, 20, 360, 25 });
	booster_.caption("부스터");
	auto booster_click = [&]
	{
		if (!booster_.checked())
		{
			eng->booster(default_value_);
		}
	};
	booster_.events().click(booster_click);

	booster_value_.move({ 20, 50, 360, 25 });
	booster_value_.caption("1.0");
	booster_value_.multi_lines(false);
	auto booster_value_click = [&]
	{
		if (booster_.checked())
		{
			float val = (float)atof(booster_value_.text().data());
			if (val)
				eng->booster(val);
		}
	};
	booster_value_.events().text_changed(booster_value_click);

	speed_.move({ 20, 80, 160, 25 });
	speed_.caption("스피드");
	speed_space_.move({ 200, 80, 160, 25 });
	speed_space_.caption("Space");

	speed_value_.move({ 20, 110, 360, 25 });
	speed_value_.caption("1.0");
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
}

mainform::~mainform()
{
	running_ = false;
	if (speed_t_.joinable())
		speed_t_.join();
}