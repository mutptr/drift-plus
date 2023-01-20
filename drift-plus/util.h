#pragma once

namespace util
{
	namespace detail
	{
		constexpr uint8_t parse_hex(char c)
		{
			if (c >= '0' && c <= '9') return c - '0';
			if ((c | 32) >= 'a' && (c | 32) <= 'f') return (c | 32) - 'a' + 10;
			return 0;
		}
	}

	uint32_t find_process_by_name(std::wstring_view name);
	uintptr_t find_module_by_name(uint32_t process_id, std::wstring_view name);

	template<size_t N>
	constexpr std::array<uint8_t, N / 3> array_from_string(const char(&str)[N])
	{
		std::array<uint8_t, N / 3> vec{};
		for (auto i = 0u; i < N; i += 3)
		{
			vec[i / 3] = (detail::parse_hex(str[i]) << 4) | detail::parse_hex(str[i + 1]);
		}
		return vec;
	}

	template<size_t I, size_t N>
	constexpr uint32_t get_4byte_from_array(std::array<uint8_t, N> arr)
	{
		return arr[I] | arr[I + 1] << 8 | arr[I + 2] << 16 | arr[I + 3] << 24;
	}
}