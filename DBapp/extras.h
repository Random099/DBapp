#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include <boost/mysql.hpp>

#include <string>
#include <string_view>
#include <iostream>
#include <cstddef>
#include <vector>


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
		constexpr char DATABASE_NAME[] = "books";

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

		void bookDatabaseTemplateSetup(std::shared_ptr<boost::mysql::tcp_connection> connPtr)
		{
			using fw = boost::mysql::field_view;
			boost::mysql::results res;

			std::string query = "CREATE DATABASE IF NOT EXISTS books";

			connPtr->execute(query, res);

			query = "CREATE TABLE IF NOT EXISTS books ("
				"book_id INT PRIMARY KEY,"
				"title VARCHAR(255),"
				"author VARCHAR(255),"
				"genre VARCHAR(50),"
				"total_copies INT,"
				"available_copies INT"
				")";

			connPtr->execute(query, res);

			query = "CREATE TABLE IF NOT EXISTS borrowers ("
				"borrower_id INT PRIMARY KEY,"
				"name VARCHAR(255),"
				"email VARCHAR(255),"
				"phone VARCHAR(20)"
				")";

			connPtr->execute(query, res);

			query = "CREATE TABLE IF NOT EXISTS transactions ("
				"transaction_id INT PRIMARY KEY,"
				"book_id INT,"
				"borrower_id INT,"
				"checkout_date DATE,"
				"due_date DATE,"
				"returned BOOLEAN,"
				"FOREIGN KEY(book_id) REFERENCES books(book_id),"
				"FOREIGN KEY(borrower_id) REFERENCES borrowers(borrower_id)"
				")";

			connPtr->execute(query, res);

			query = "CREATE TABLE IF NOT EXISTS authors ("
				"author_id INT PRIMARY KEY,"
				"author_name VARCHAR(255)"
				")";

			connPtr->execute(query, res);

			query = "CREATE TABLE IF NOT EXISTS genres ("
				"genre_id INT PRIMARY KEY,"
				"genre_name VARCHAR(50)"
				")";

			connPtr->execute(query, res);

			query = "CREATE TABLE IF NOT EXISTS book_authors ("
				"book_id INT,"
				"author_id INT,"
				"PRIMARY KEY(book_id, author_id),"
				"FOREIGN KEY(book_id) REFERENCES books(book_id),"
				"FOREIGN KEY(author_id) REFERENCES authors(author_id)"
				")";

			connPtr->execute(query, res);

			query = "CREATE TABLE IF NOT EXISTS book_genres ("
				"book_id INT,"
				"genre_id INT,"
				"PRIMARY KEY(book_id, genre_id),"
				"FOREIGN KEY(book_id) REFERENCES books(book_id),"
				"FOREIGN KEY(genre_id) REFERENCES genres(genre_id)"
				")";

			connPtr->execute(query, res);

			boost::mysql::statement stmt;
			std::vector<std::vector<fw> > data_1 = {
					{fw(1), fw("Book 1"), fw("Author 1"), fw("Fiction"), fw(10), fw(10)},
					{fw(2), fw("Book 2"), fw("Author 2"), fw("Non-Fiction"), fw(15), fw(15)},
					{fw(3), fw("Book 3"), fw("Author 3"), fw("Science Fiction"), fw(20), fw(20)}
			};
			stmt = connPtr->prepare_statement("INSERT INTO books (book_id, title, author, genre, total_copies, available_copies) VALUES (?, ?, ?, ?, ?, ?)");
			for (std::vector<fw> row : data_1)
				connPtr->execute(stmt.bind(row.begin(), row.end()), res);


			std::vector<std::vector<fw> > data_2 = {
					{fw(1), fw("Borrower 1"), fw("borrower1@example.com"), fw("1234567890")},
					{fw(2), fw("Borrower 2"), fw("borrower2@example.com"), fw("9876543210")},
					{fw(3), fw("Borrower 3"), fw("borrower3@example.com"), fw("5678901234")}
			};
			stmt = connPtr->prepare_statement("INSERT INTO borrowers (borrower_id, name, email, phone) VALUES (?, ?, ?, ?)");
			for (std::vector<fw> row : data_2)
				connPtr->execute(stmt.bind(row.begin(), row.end()), res);


			std::vector<std::vector<fw> > data_3 = {
					{fw(1), fw(1), fw(1), fw("2024-01-01"), fw("2024-01-15"), fw(false)},
					{fw(2), fw(2), fw(2), fw("2024-01-02"), fw("2024-01-16"), fw(false)},
					{fw(3), fw(3), fw(3), fw("2024-01-03"), fw("2024-01-17"), fw(false)}
			};
			stmt = connPtr->prepare_statement("INSERT INTO transactions (transaction_id, book_id, borrower_id, checkout_date, due_date, returned) VALUES (?, ?, ?, ?, ?, ?)");
			for (std::vector<fw> row : data_3)
				connPtr->execute(stmt.bind(row.begin(), row.end()), res);


			std::vector<std::vector<fw> > data_4 = {
					{fw(1), fw("Author 1")},
					{fw(2), fw("Author 2")},
					{fw(3), fw("Author 3")}
			};
			stmt = connPtr->prepare_statement("INSERT INTO authors (author_id, author_name) VALUES (?, ?)");
			for (std::vector<fw> row : data_4)
				connPtr->execute(stmt.bind(row.begin(), row.end()), res);


			std::vector<std::vector<boost::mysql::field_view> > data_5 = {
					{fw(1), fw("Fiction")},
					{fw(2), fw("Non-Fiction")},
					{fw(3), fw("Science Fiction")}
			};
			stmt = connPtr->prepare_statement("INSERT INTO genres (genre_id, genre_name) VALUES (?, ?)");
			for (std::vector<fw> row : data_5)
				connPtr->execute(stmt.bind(row.begin(), row.end()), res);


			std::vector<std::vector<boost::mysql::field_view> > data_6 = {
					{fw(1), fw(1)},
					{fw(2), fw(2)},
					{fw(3), fw(3)}
			};
			stmt = connPtr->prepare_statement("INSERT INTO book_authors (book_id, author_id) VALUES (?, ?)");
			for (std::vector<fw> row : data_6)
				connPtr->execute(stmt.bind(row.begin(), row.end()), res);


			std::vector<std::vector<boost::mysql::field_view> > data_7 = {
					{fw(1), fw(1)},
					{fw(2), fw(2)},
					{fw(3), fw(3)}
			};
			stmt = connPtr->prepare_statement("INSERT INTO book_genres (book_id, genre_id) VALUES (?, ?)");
			for (std::vector<fw> row : data_7)
				connPtr->execute(stmt.bind(row.begin(), row.end()), res);
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