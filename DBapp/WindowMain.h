#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"

#include <string>
#include <memory>

#include <boost/mysql.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>

class WindowMain
{
private:
	std::shared_ptr<boost::asio::io_context> m_ioContextPtr;
	std::shared_ptr<boost::mysql::tcp_connection> m_connPtr;
public:
	WindowMain(std::shared_ptr<boost::asio::io_context>, std::shared_ptr<boost::mysql::tcp_connection>);
	void display();
};
