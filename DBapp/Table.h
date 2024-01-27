#pragma once
#include "extras.h"

#include "imgui.h"
#include "imgui_stdlib.h"

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <map>

#include <boost/mysql.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

class Table
{
public:
	Table(std::shared_ptr<boost::mysql::tcp_connection>, std::shared_ptr<std::string>, const std::vector<std::string>&);
	void display();
	bool* displaying();
	void clear();
private:
	void displayInputMenu();
	void displayHeaders();
	void displayData();
	std::shared_ptr<std::map<std::string, std::string> > m_toInput;
	std::shared_ptr<boost::mysql::tcp_connection> m_connPtr;
	std::shared_ptr<std::string> m_namePtr;
	std::vector<std::string> m_columnNames;
	boost::mysql::results m_queryResult;
	bool* m_display;
	std::pair<std::string, constants::SORT_ORDER> m_sortOrder;
};
