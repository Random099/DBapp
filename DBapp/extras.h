#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include <boost/mysql.hpp>

#include <string>
#include <string_view>

namespace constants
{
	namespace
	{
		constexpr char HOST[] = "localhost";
		constexpr char PORT[] = "3306";
		constexpr char DATABASE[] = "test";

		constexpr ImVec2 LOGIN_WINDOW_SIZE = { 300, 200 };
	}
}

inline std::string bsvToString(boost::mysql::string_view bsv) {
	return std::string(bsv.data(), bsv.size());
}
