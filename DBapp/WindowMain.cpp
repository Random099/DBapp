#include "WindowMain.h"

WindowMain::WindowMain(std::shared_ptr<boost::asio::io_context> ioContextPtr, std::shared_ptr<boost::mysql::tcp_connection> connPtr) :
	m_ioContextPtr(ioContextPtr),
	m_connPtr(connPtr)
{}

void WindowMain::display()
{
	ImGui::Begin("Menu");

	ImGui::Text("We connected HEHE!");

	ImGui::End();
}