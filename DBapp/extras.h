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
		enum class SORT_ORDER 
		{
			desc = 1 << 1,
			asc = 1 << 2
		};
		constexpr char HOST[] = "localhost";
		constexpr char PORT[] = "3306";
		constexpr char DATABASE_NAME[] = "test";

		constexpr ImVec2 LOGIN_WINDOW_SIZE = { 300, 200 };
		constexpr ImVec2 TABLE_WINDOW_SIZE = { 800, 600 };
		constexpr ImVec2 INPUT_MENU_SIZE = { 800, 80 };
		constexpr ImGuiWindowFlags MAIN_WINDOW_FLAGS = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus;
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
		inline std::string showTableSort(const std::string& tableName, const std::string& columnName, constants::SORT_ORDER sortOrder)
		{
			switch (sortOrder)
			{
				case constants::SORT_ORDER::asc:
					return "SELECT * FROM " + tableName + " ORDER BY " + columnName + " ASC";
					break;
				case constants::SORT_ORDER::desc:
					return "SELECT * FROM " + tableName + " ORDER BY " + columnName + " DESC";
					break;
			}
			return "";
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