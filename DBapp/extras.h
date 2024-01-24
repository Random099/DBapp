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
		constexpr ImVec2 TABLE_WINDOW_SIZE = { 800, 600 };
	}
}

namespace queries
{
	namespace
	{
		constexpr char SHOW_TABLES[] = "SHOW TABLES";
		inline std::string showTable(const std::string& tableName)
		{
			return "SELECT * FROM " + tableName;
		}
	}
}

namespace helpers
{
	namespace
	{
		inline std::string bsvToString(boost::mysql::string_view bsv) {
			return std::string(bsv.data(), bsv.size());
		}

		void fieldTypeMap(const boost::mysql::field_view& field)
		{
			switch (field.kind())
			{
			case boost::mysql::field_kind::int64:
				ImGui::Text("%lld", field.as_int64());
				break;
			case boost::mysql::field_kind::uint64:
				ImGui::Text("%llu", field.as_uint64());
				break;
			case boost::mysql::field_kind::string:
				ImGui::Text("%s", bsvToString(field.as_string()).c_str());
				break;
			case boost::mysql::field_kind::float_:
				ImGui::Text("%f", field.as_double());
				break;
			case boost::mysql::field_kind::double_:
				ImGui::Text("%f", field.as_float());
				break;
			case boost::mysql::field_kind::null:
				ImGui::Text("NULL");
				break;
			}
		}
	}
}