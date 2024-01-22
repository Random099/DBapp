#include "WindowLogin.h"

WindowLogin::WindowLogin(std::shared_ptr<boost::asio::io_context> ioContextPtr, std::shared_ptr<boost::mysql::tcp_connection> connPtr, const std::string& address, const std::string& port) :
	m_ioContextPtr(ioContextPtr),
	m_connPtr(connPtr),
	m_address(address),
	m_port(port),
	m_username(""),
	m_password(""),
	m_loginNotSuccesful(true)
{}

bool WindowLogin::display() 
{
	ImGui::Begin("Login");

	ImGui::Text("Username");
	ImGui::InputText("##username", &m_username);
	ImGui::Text("Password");
	ImGui::InputText("##password", &m_password);
	if (ImGui::Button("Submit"))
	{
		if (!m_username.empty())
		{
			try
			{
				std::cout << m_address << m_port << m_username << m_password << '\n';
				if (m_password.empty())
				{
					boost::asio::ip::tcp::resolver resolver(m_ioContextPtr->get_executor());
					auto endpoints = resolver.resolve(constants::HOST, constants::PORT);

					boost::mysql::handshake_params params(
						"root", 
						"root",                
						"test"  
					);

					m_connPtr->connect(*endpoints.begin(), params);
				}
				else
				{
					boost::asio::ip::tcp::resolver resolver(m_ioContextPtr->get_executor());
					auto endpoints = resolver.resolve(constants::HOST, constants::PORT);

					boost::mysql::handshake_params params(
						m_username,                
						m_password,                
						constants::DATABASE  
					);

					m_connPtr->connect(*endpoints.begin(), params);
				}
				m_loginNotSuccesful = false;
			}
			catch (const boost::mysql::error_with_diagnostics& err)
			{
				std::cout << err.get_diagnostics().server_message() << '\n';
				m_username.clear();
				m_password.clear();
			}
		}
	}

	ImGui::End();

	return m_loginNotSuccesful;
}