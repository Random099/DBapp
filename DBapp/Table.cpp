#include "Table.h"

Table::Table(std::shared_ptr<boost::mysql::tcp_connection> connPtr,
			std::shared_ptr<std::string> namePtr,
			const std::vector<std::string>& columnNames) : 
	m_toInput{ std::make_shared<std::map<std::string, std::string> >() },
	m_connPtr{ connPtr },
	m_namePtr{ namePtr },
	m_columnNames{ columnNames },
	m_display{ new bool(true) },
	m_sortOrder{ std::make_pair("", constants::SORT_ORDER::asc) }
{
	m_connPtr->query(queries::showTable(*m_namePtr), m_queryResult);
}

void Table::display()
{
	if (!ImGui::Begin(m_namePtr->c_str(), m_display, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}
	ImGui::SetWindowSize(constants::TABLE_WINDOW_SIZE);

	this->displayInputMenu();

	if (ImGui::BeginTable(m_namePtr->c_str(), static_cast<int>(m_queryResult.rows().at(0).size()), ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders))
	{
		for (const std::string& columnName : m_columnNames)
		{
			ImGui::TableSetupColumn(columnName.c_str());
		}
		ImGui::TableSetupScrollFreeze(0, 1);

		this->displayHeaders();
		this->displayData();

		ImGui::EndTable();
	}

	ImGui::End();
}

bool* Table::displaying()
{
	return m_display;
}

void Table::clear()
{
	m_connPtr = nullptr;
	m_namePtr = nullptr;
	m_toInput = nullptr;
}

void Table::displayInputMenu()
{
	if (ImGui::BeginMenuBar())
	{
		ImGui::SetNextWindowSize(constants::INPUT_MENU_SIZE);
		if (ImGui::BeginMenu("Insert"))
		{
			for (int column = 0; column < m_columnNames.size(); column++)
			{
				ImGui::SetNextItemWidth(constants::TABLE_WINDOW_SIZE.x / static_cast<float>(m_columnNames.size()));
				std::string label = "##" + m_columnNames[column];
				ImGui::InputTextWithHint(label.c_str(), m_columnNames[column].c_str(), &(*m_toInput)[m_columnNames[column]]);
				ImGui::SameLine();
			}
			ImGui::NewLine();
			if(ImGui::Button("Submit"))
			{

			}
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				for (auto& [key, value] : *m_toInput)
				{
					value.clear();
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void Table::displayHeaders()
{
	ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
	for (int column = 0; column < m_columnNames.size(); column++)
	{
		ImGui::TableSetColumnIndex(column);
		if (ImGui::Button(m_columnNames[column].c_str()))
		{
			if (m_sortOrder.first == m_columnNames[column])
			{
				if (m_sortOrder.second == constants::SORT_ORDER::desc)
				{
					m_connPtr->query(queries::showTableSort(*m_namePtr, m_columnNames[column].c_str(), constants::SORT_ORDER::asc), m_queryResult);
					m_sortOrder = std::make_pair(m_columnNames[column], constants::SORT_ORDER::asc);
				}
				else
				{
					m_connPtr->query(queries::showTableSort(*m_namePtr, m_columnNames[column].c_str(), constants::SORT_ORDER::desc), m_queryResult);
					m_sortOrder = std::make_pair(m_columnNames[column], constants::SORT_ORDER::desc);
				}
			}
			else
			{
				m_connPtr->query(queries::showTableSort(*m_namePtr, m_columnNames[column].c_str(), constants::SORT_ORDER::asc), m_queryResult);
				m_sortOrder = std::make_pair(m_columnNames[column], constants::SORT_ORDER::asc);
			}
		}
		//ImGui::TableHeader(column_name);
	}
}

void Table::displayData()
{
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
}