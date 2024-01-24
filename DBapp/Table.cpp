#include "Table.h"

Table::Table(std::shared_ptr<boost::mysql::tcp_connection> connPtr, const std::string& name, const std::vector<std::string>& columnNames) : 
	m_connPtr(connPtr),
	m_name(name),
	m_columnNames(columnNames)
{
	m_connPtr->query(queries::showTable(m_name), m_queryResult);
}

void Table::display()
{
	ImGui::Begin(m_name.c_str());
	ImGui::SetWindowSize(constants::TABLE_WINDOW_SIZE);
	ImGui::SetWindowFocus(m_name.c_str());

	if (ImGui::BeginTable(m_name.c_str(), static_cast<int>(m_queryResult.rows().at(0).size()), ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		for (const std::string& columnName : m_columnNames)
		{
			ImGui::TableSetupColumn(columnName.c_str());
		}
		ImGui::TableHeadersRow();
		for (boost::mysql::row_view row : m_queryResult.rows())
		{
			ImGui::TableNextRow();
			int columnIndex = 0;
			for (const boost::mysql::field_view& field : row)
			{
				ImGui::TableSetColumnIndex(columnIndex++);
				helpers::fieldTypeMap(field);
			}
		}
		ImGui::EndTable();
	}

	ImGui::End();
}