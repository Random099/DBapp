#include "WindowMain.h"

WindowMain::WindowMain(std::shared_ptr<ImVec2> screenSizePtr,
					std::shared_ptr<boost::asio::io_context> ioContextPtr,
					std::shared_ptr<boost::mysql::tcp_connection> connPtr) :
	m_screenSizePtr{ screenSizePtr },
	m_ioContextPtr{ ioContextPtr },
	m_connPtr{ connPtr },
	m_columnNamesMapPtr{ nullptr },
	m_mappingTables{ true },
	m_viewport{ ImGui::GetMainViewport() }
{}

void WindowMain::display()
{
	ImGui::SetNextWindowPos(m_viewport->Pos);
	ImGui::SetNextWindowSize(m_viewport->Size);
	if (!ImGui::Begin("##Fullscreen window", NULL, constants::MAIN_WINDOW_FLAGS))
	{
		ImGui::End();
		return;
	}
	
	ImGui::Text("Display table");
	
	this->handleTables();
	
	ImGui::End();
}

void WindowMain::handleTables()
{
	if (m_mappingTables)
	{
		this->columnNamesMap();
	}

	if (ImGui::Button("Display table"))
	{
		for (auto& [tableName, columnNames] : *m_columnNamesMapPtr)
		{
				m_mappingTables = true;
				m_tables[tableName] = (std::make_shared<Table>(
				m_connPtr,
				std::make_shared<std::string>(tableName),
				columnNames));
		}
	}

	std::vector<std::string> tablesToRemove;
	for (auto& [name, table] : m_tables)
	{
		if (*(table->displaying()))
		{
			table->display();
		}
		else
		{
			tablesToRemove.push_back(name);
		}
	}
	for (const std::string& name : tablesToRemove)
	{
		m_tables.erase(name);
	}
}

void WindowMain::columnNamesMap()
{
	m_columnNamesMapPtr = std::make_shared<std::map<std::string, std::vector<std::string> > >();
	boost::mysql::results result;
	boost::mysql::results result2;

	m_connPtr->query(queries::SHOW_TABLES , result);
	for (int j = 0; j < result.rows().size(); ++j)
	{
		std::string query2 = std::string("DESCRIBE " + helpers::bsvToString(result.rows()[0][j].as_string())).c_str();
		m_connPtr->query(query2, result2);
		for (int i = 0; i < result2.rows().size(); ++i)
		{
			(*m_columnNamesMapPtr)[result.rows()[0][j].as_string()].push_back(result2.rows()[i][0].as_string());
		}
	}
	m_mappingTables = false;
}

