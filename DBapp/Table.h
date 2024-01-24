#pragma once
#include "extras.h"

#include "imgui.h"
#include "imgui_stdlib.h"

#include <string>
#include <vector>
#include <memory>

#include <boost/mysql.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

class Table
{
public:
	Table(std::shared_ptr<boost::mysql::tcp_connection>, const std::string&, const std::vector<std::string>&);
	void display();
private:
	std::shared_ptr<boost::mysql::tcp_connection> m_connPtr;
	std::string m_name;
	std::vector<std::string> m_columnNames;
	boost::mysql::results m_queryResult;
};

