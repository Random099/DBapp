#include "WindowMain.h"

WindowMain::WindowMain(std::shared_ptr<ImVec2> screenSizePtr, std::shared_ptr<boost::asio::io_context> ioContextPtr, std::shared_ptr<boost::mysql::tcp_connection> connPtr) :
	m_screenSizePtr(screenSizePtr),
	m_ioContextPtr(ioContextPtr),
	m_connPtr(connPtr)
{}

void WindowMain::display()
{
	ImGui::Begin("Menu");
	ImGui::SetWindowSize(*m_screenSizePtr);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	
	ImGui::Text("Display table");
	
	if (ImGui::Button("Display table"))
	{
		const char* sql = "SELECT * FROM drzewo";
		m_connPtr->ping();
		m_connPtr->query(sql, m_queryResult);
		displayingTable = true;
	}
	if (displayingTable)
	{
		ImGui::Begin("Table");
		ImGui::SetWindowFocus("Table");

		std::cout << "COLUMNS: " << m_queryResult.rows().at(0).size() << '\n';

		auto x = m_queryResult.meta()[2];
		std::cout << "NAME: " << x.column_name().at(1) << '\n';
		if (ImGui::BeginTable("table1", m_queryResult.rows().at(0).size()))
		{
			for (boost::mysql::row_view row : m_queryResult.rows())
			{
				ImGui::TableNextRow();
				int columnIndex = 0;
				for (const boost::mysql::field_view& field : row)
				{
					ImGui::TableSetColumnIndex(columnIndex++);
					mapField(field);
				}
			}
			ImGui::EndTable();
		}
	
		ImGui::End();
	}
	ImGui::End();
}

void WindowMain::mapField(const boost::mysql::field_view& field) const
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