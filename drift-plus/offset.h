#pragma once

namespace offset
{
	namespace client
	{
		constexpr uint32_t crash_guard = 0x140565E;
		constexpr uint32_t smooth_drift = 0x1479004;
		constexpr uint32_t body1 = 0x39E0C70;
		constexpr uint32_t body2 = 0x39E03E0;
		constexpr uint32_t booster = 0x1404929;
		constexpr uint32_t team_booster = 0x14089B7;
	}
	namespace client::physx3
	{
		constexpr uint32_t speed = 0x193FE6;
	}
}