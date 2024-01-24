#include "WindowMain.h"

WindowMain::WindowMain(std::shared_ptr<ImVec2> screenSizePtr, std::shared_ptr<boost::asio::io_context> ioContextPtr, std::shared_ptr<boost::mysql::tcp_connection> connPtr) :
	m_screenSizePtr(screenSizePtr),
	m_ioContextPtr(ioContextPtr),
	m_connPtr(connPtr),
	m_columnNamesMapPtr(nullptr)
{}

void WindowMain::display()
{
	ImGui::Begin("Menu");
	ImGui::SetWindowSize(*m_screenSizePtr);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	
	ImGui::Text("Display table");
	
	if (mappingTables)
	{
		this->columnNamesMap();
	}
	if (ImGui::Button("Display table"))
	{
		this->columnNamesMap();
		for (auto& [tableName, columnNames] : *m_columnNamesMapPtr)
		{
			this->m_tables.push_back(std::make_shared<Table>(m_connPtr, tableName, columnNames));
		}
		displayingTable = true;
	}

	if (displayingTable)
	{
		for (std::shared_ptr<Table> table : m_tables)
		{
			table->display();
		}
	}

	ImGui::End();
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
}

