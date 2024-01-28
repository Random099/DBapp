#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include <boost/mysql.hpp>

#include <string>
#include <string_view>
#include <iostream>
#include <cstddef>

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

		inline std::string idDeleteTemplate(const std::string& tableName, const int& id)
		{
			return "DELETE FROM " + tableName + " WHERE id=" + std::to_string(id);
		}

		inline std::string idUpdateTemplate(const std::string& tableName, const std::string& columnName, const int& id)
		{
			std::string result = "UPDATE " + tableName + " SET ";
			result += columnName + "=(?) ";
			result += "WHERE id=" + std::to_string(id);
			return result;
		}

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
		inline std::string insertStatementTemplate(const std::string& tableName, const std::vector<std::string>& columnNames)
		{
			std::string result = "INSERT INTO " + tableName + " (";
			int columnCount = static_cast<int>(columnNames.size());
			for (int i = 0; i < columnCount; ++i)
			{
				if (i < columnCount - 1)
					result += columnNames[i] + ", ";
				else
					result += columnNames[i] + ") ";
			}
			result += "VALUES (";
			for (int i = 0; i < columnCount; ++i)
			{
				if (i < columnCount - 1)
					result += "?, ";
				else
					result += "?) ";
			}
			return result;
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
			case boost::mysql::field_kind::time:
				ImGui::Text("TIME PLACEHOLDER");
				break;
			case boost::mysql::field_kind::datetime:
				ImGui::Text("DATETIME PLACEHOLDER");
				break;
			case boost::mysql::field_kind::blob:
				ImGui::Text("BLOB PLACEHOLDER");
				break;
			case boost::mysql::field_kind::null:
				ImGui::Text("NULL");
				break;
			default:
				ImGui::Text("UNKNOWN");
				break;
			}
		}

		std::string stringKind(const boost::mysql::field_kind& fieldKind)
		{
			switch (fieldKind)
			{
			case boost::mysql::field_kind::int64:
				return "INT64";
				break;
			case boost::mysql::field_kind::uint64:
				return "UINT64";
				break;
			case boost::mysql::field_kind::string:
				return "STRING";
				break;
			case boost::mysql::field_kind::float_:
				return "FLOAT";
				break;
			case boost::mysql::field_kind::double_:
				return "DOUBLE";
				break;
			case boost::mysql::field_kind::time:
				return "TIME PLACEHOLDER";
				break;
			case boost::mysql::field_kind::datetime:
				return "DATETIME PLACEHOLDER";
				break;
			case boost::mysql::field_kind::blob:
				return "BLOB PLACEHOLDER";
				break;
			case boost::mysql::field_kind::null:
				return "NULL";
				break;
			default:
				return "UNKNOWN";
				break;
			}
		}

		boost::mysql::field_view toFieldView(const std::string& data, boost::mysql::field_kind kind)
		{
			switch (kind)
			{
			case boost::mysql::field_kind::int64:
				return boost::mysql::field_view(std::stoll(data));
				break;
			case boost::mysql::field_kind::uint64:
				return boost::mysql::field_view(std::stoull(data));
				break;
			case boost::mysql::field_kind::string:
				return boost::mysql::field_view(data);
				break;
			case boost::mysql::field_kind::float_:
				return boost::mysql::field_view(std::stof(data));
				break;
			case boost::mysql::field_kind::double_:
				return boost::mysql::field_view(std::stod(data));
				break;
			case boost::mysql::field_kind::time:
				return boost::mysql::field_view("TIME PLACEHOLDER");
				break;
			case boost::mysql::field_kind::datetime:
				return boost::mysql::field_view("DATETIME PLACEHOLDER");
				break;
			case boost::mysql::field_kind::blob:
				return boost::mysql::field_view("BLOB PLACEHOLDER");
				break;
			case boost::mysql::field_kind::null:
				return boost::mysql::field_view();
				break;
			default:
				return boost::mysql::field_view("UNKNOWN");
				break;
			}
		}

		std::string fieldViewToStr(const boost::mysql::field_view& field)
		{
			switch (field.kind())
			{
			case boost::mysql::field_kind::int64:
				return std::to_string(field.as_int64());
				break;
			case boost::mysql::field_kind::uint64:
				return std::to_string(field.as_uint64());
				break;
			case boost::mysql::field_kind::string:
				return field.as_string();
				break;
			case boost::mysql::field_kind::float_:
				return std::to_string(field.as_double());
				break;
			case boost::mysql::field_kind::double_:
				return std::to_string(field.as_float());
				break;
			case boost::mysql::field_kind::time:
				return "TIME PLACEHOLDER";
				break;
			case boost::mysql::field_kind::datetime:
				return "DATETIME PLACEHOLDER";
				break;
			case boost::mysql::field_kind::blob:
				return "BLOB PLACEHOLDER";
				break;
			case boost::mysql::field_kind::null:
				return "NULL";
				break;
			default:
				return "UNKNOWN";
				break;
			}
		}
	}
}