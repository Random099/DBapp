#include "Table.h"

Table::Table(std::shared_ptr<boost::mysql::tcp_connection> connPtr,
			std::shared_ptr<std::string> namePtr,
			const std::vector<std::string>& columnNames) : 
	m_selected{ nullptr },
	m_toUpdate{ std::make_shared<std::string>("") },
	m_toInput{ std::make_shared<std::map<std::string, std::string> >() },
	m_connPtr{ connPtr },
	m_namePtr{ namePtr },
	m_columnNames{ columnNames },
	m_display{ new bool(true) },
	m_sortOrder{ std::make_pair("", constants::SORT_ORDER::asc) }
{
	m_connPtr->query(queries::showTable(*m_namePtr), m_queryResult);
	int columnIdx = 0;
	for (const boost::mysql::field_view& field : m_queryResult.rows()[0])
	{
		std::cout << m_columnNames[columnIdx] << '\n';
		m_columnTypes[m_columnNames[columnIdx]] = field.kind();
		++columnIdx;
	}
	columnIdx = 0;
	for (auto& [column, type] : m_columnTypes)
	{ 
		std::cout << columnIdx << " " << column << " ";
		std::cout << helpers::stringKind(type);
		++columnIdx;
	}
	std::cout << '\n';
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
				std::string hint = std::string(m_columnNames[column] + "(" + helpers::stringKind(m_columnTypes[m_columnNames[column]])) + ")";
				ImGui::InputTextWithHint(label.c_str(), hint.c_str(), &(*m_toInput)[m_columnNames[column]]);
				ImGui::SameLine();
			}
			ImGui::NewLine();
			if(ImGui::Button("Submit"))
			{
				std::cout << "QUERY: " << queries::insertStatementTemplate(*m_namePtr, m_columnNames) << '\n';
				boost::mysql::statement stmt = m_connPtr->prepare_statement(
					queries::insertStatementTemplate(*m_namePtr, m_columnNames)
				);
				std::vector<boost::mysql::field_view> values;
				try
				{
					for (auto& [column, value] : *m_toInput)
					{
						std::cout << "INSERTING: " << column << " " << value << helpers::stringKind(helpers::toFieldView(value, m_columnTypes[column]).kind()) << '\n';
						values.push_back(helpers::toFieldView(value, m_columnTypes[column]));
					}
					m_connPtr->execute(stmt.bind(values.begin(), values.end()), m_queryResult);
					m_connPtr->query(queries::showTable(*m_namePtr), m_queryResult);
				}
				catch (const boost::mysql::error_with_diagnostics& err)
				{
					for (auto& [key, value] : *m_toInput)
						value.clear();
					std::cout << "Operation failed with error code: " << err.code() << '\n'
						<< "Server diagnostics: " << err.get_diagnostics().server_message() << '\n';
				}
				catch (const boost::exception& boostException)
				{
					for (auto& [key, value] : *m_toInput)
						value.clear();
					std::cerr << "boost error: " << boost::diagnostic_information(boostException) << '\n';
				}
				catch(const std::exception& e)
				{
					for (auto& [key, value] : *m_toInput)
						value.clear();
					std::cerr << "std error: " << e.what() << '\n';
				}
				catch(...)
				{
					for (auto& [key, value] : *m_toInput)
						value.clear();
					std::cerr << "unknown error\n";
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				for (auto& [key, value] : *m_toInput)
					value.clear();
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
	int fieldIdx = 0;
	for (boost::mysql::row_view row : m_queryResult.rows())
	{
		ImGui::TableNextRow();
		int columnIndex = 0;
		for (const boost::mysql::field_view& field : row)
		{
			ImGui::TableSetColumnIndex(columnIndex);
			if (columnIndex == 0)
			{
				std::string treeLabel = helpers::fieldViewToStr(field);
				if (ImGui::TreeNode(treeLabel.c_str()))
				{
					if (ImGui::Button("Delete"))
					{
						m_connPtr->query(queries::idDeleteTemplate(*m_namePtr, field.as_int64()), m_queryResult);
						m_connPtr->query(queries::showTable(*m_namePtr), m_queryResult);
					}
					ImGui::TreePop();
				}
			}
			else
			{
				std::string treeLabel = helpers::fieldViewToStr(field);
				if (ImGui::TreeNode((treeLabel+"##"+std::to_string(fieldIdx)).c_str()))
				{
					std::string label = "##" +	std::to_string(fieldIdx);
					std::string hint = std::string(m_columnNames[columnIndex] + "(" + helpers::stringKind(m_columnTypes[m_columnNames[columnIndex]])) + ")";
					ImGui::InputTextWithHint(label.c_str(), hint.c_str(), &(*m_toUpdate));
					ImGui::SameLine();
					if (ImGui::Button("Update"))
					{
						try
						{
							std::cout << "QUERY: " << queries::idUpdateTemplate(*m_namePtr, m_columnNames[columnIndex], row[0].as_int64()) << '\n';
							boost::mysql::statement stmt = m_connPtr->prepare_statement(
								queries::idUpdateTemplate(*m_namePtr, m_columnNames[columnIndex], row[0].as_int64())
							);
							m_connPtr->execute(stmt.bind(helpers::toFieldView(*m_toUpdate, field.kind())), m_queryResult);
							m_connPtr->query(queries::showTable(*m_namePtr), m_queryResult);
							m_toUpdate->clear();
						}
						catch (const boost::mysql::error_with_diagnostics& err)
						{
							m_toUpdate->clear();
							std::cout << "Operation failed with error code: " << err.code() << '\n'
								<< "Server diagnostics: " << err.get_diagnostics().server_message() << '\n';
						}
						catch (const boost::exception& boostException)
						{
							m_toUpdate->clear();
							std::cerr << "boost error: " << boost::diagnostic_information(boostException) << '\n';
						}
						catch (const std::exception& e)
						{
							m_toUpdate->clear();
							std::cerr << "std error: " << e.what() << '\n';
						}
						catch (...)
						{
							m_toUpdate->clear();
							std::cerr << "unknown error\n";
						}
					}
					ImGui::TreePop();
				}
			}
			++columnIndex;
			++fieldIdx;
		}
	}
}