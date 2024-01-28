#include "WindowLogin.h"

WindowLogin::WindowLogin(std::shared_ptr<ImVec2> screenSizePtr, std::shared_ptr<boost::asio::io_context> ioContextPtr, std::shared_ptr<boost::mysql::tcp_connection> connPtr) :
	m_ioContextPtr(ioContextPtr),
	m_connPtr(connPtr),
	m_username(""),
	m_password(""),
	m_loginNotSuccesful(true)
{
	m_windowPos = ImVec2(screenSizePtr->x / 2.0f - constants::LOGIN_WINDOW_SIZE.x / 2.0f, screenSizePtr->y / 2.0f - constants::LOGIN_WINDOW_SIZE.y / 2.0f);
}

bool WindowLogin::display() 
{
	ImGui::Begin("Login");
	ImGui::SetWindowSize(constants::LOGIN_WINDOW_SIZE);
	ImGui::SetWindowPos(m_windowPos);

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
				boost::asio::ip::tcp::resolver resolver(m_ioContextPtr->get_executor());
				auto endpoints = resolver.resolve(constants::HOST, constants::PORT);
				std::unique_ptr<boost::mysql::handshake_params> paramsPtr(nullptr);
				if (m_password.empty())
				{
					paramsPtr = std::make_unique<boost::mysql::handshake_params>(
						"root",
						"root",
						constants::DATABASE_NAME
					);
				}
				else
				{
					paramsPtr = std::make_unique<boost::mysql::handshake_params>(
						m_username,                
						m_password,                
						constants::DATABASE_NAME  
					);
				}
				m_connPtr->connect(*endpoints.begin(), *paramsPtr);
				queries::bookDatabaseTemplateSetup(m_connPtr);
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