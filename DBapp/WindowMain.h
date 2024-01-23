#pragma once

#include "extras.h"

#include "imgui.h"
#include "imgui_stdlib.h"

#include <string>
#include <memory>
#include <iostream>

#include <boost/mysql.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>

class WindowMain
{
private:
	void mapField(const boost::mysql::field_view&) const;
	std::shared_ptr<boost::asio::io_context> m_ioContextPtr;
	std::shared_ptr<boost::mysql::tcp_connection> m_connPtr;
	std::shared_ptr<ImVec2> m_screenSizePtr;
	boost::mysql::results m_queryResult;
	bool displayingTable = false;
public:
	WindowMain(std::shared_ptr<ImVec2>, std::shared_ptr<boost::asio::io_context>, std::shared_ptr<boost::mysql::tcp_connection>);
	void display();
};
