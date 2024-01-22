#pragma once

#include "WindowLogin.h"
#include "WindowMain.h"
#include "extras.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <utility>

#include <boost/mysql.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>

class Interface
{
public:
    Interface();
    ~Interface();
    int run();
private:
    void prepareImGuiStyles();

    std::shared_ptr<boost::asio::io_context> m_ioContextPtr;
    std::shared_ptr<boost::mysql::tcp_connection> m_connPtr;
    bool m_notLoggedIn;
};
