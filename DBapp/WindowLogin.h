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

class WindowLogin
{
private:
	std::shared_ptr<boost::asio::io_context> m_ioContextPtr;
	std::shared_ptr<boost::mysql::tcp_connection> m_connPtr;
	ImVec2 m_windowPos;
	bool m_loginNotSuccesful;
	std::string m_username;
	std::string m_password;
public:
	WindowLogin(std::shared_ptr<ImVec2>, std::shared_ptr<boost::asio::io_context>, std::shared_ptr<boost::mysql::tcp_connection>);
	bool display();
};
