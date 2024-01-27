#pragma once

#include "extras.h"
#include "Table.h"

#include "imgui.h"
#include "imgui_stdlib.h"

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <map>
#include <tuple>

#include <boost/mysql.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

class WindowMain
{
private:
	void handleTables();

	void columnNamesMap();
	std::map<std::string, std::shared_ptr<Table> > m_tables;
	std::shared_ptr<boost::asio::io_context> m_ioContextPtr;
	std::shared_ptr<boost::mysql::tcp_connection> m_connPtr;
	std::shared_ptr<ImVec2> m_screenSizePtr;
	std::shared_ptr<std::map<std::string, std::vector<std::string> > > m_columnNamesMapPtr;
	boost::mysql::results m_queryResult;
	bool m_mappingTables;
	const ImGuiViewport* m_viewport;
public:
	WindowMain(std::shared_ptr<ImVec2>, std::shared_ptr<boost::asio::io_context>, std::shared_ptr<boost::mysql::tcp_connection>);
	void display();
};
