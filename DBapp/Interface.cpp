#include "Interface.h"

Interface::Interface() :
	m_ioContextPtr{ std::make_shared<boost::asio::io_context>() },
	m_notLoggedIn{ true }
{
	m_connPtr = std::make_shared<boost::mysql::tcp_connection>(m_ioContextPtr->get_executor());
}

Interface::~Interface()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

int Interface::run()
{
	if (!glfwInit())
		return -1;
	std::shared_ptr<ImVec2> screenResolution = this->resolutionGet();
	GLFWwindow* window = glfwCreateWindow(static_cast<int>(screenResolution->x), static_cast<int>(screenResolution->y), "DBapp", NULL, nullptr);
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { glfwSetWindowShouldClose(window, GLFW_FALSE); });
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	gl3wInit();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGuiIO& io = ImGui::GetIO();

	WindowLogin windowLogin{ screenResolution, m_ioContextPtr, m_connPtr };
	WindowMain windowMain{ screenResolution, m_ioContextPtr, m_connPtr };

	//this->prepareImGuiStyles();

	for (;;) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (m_notLoggedIn)
		{
			m_notLoggedIn = windowLogin.display();
		}
		else
		{
			windowMain.display();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}

void Interface::prepareImGuiStyles()
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.188f, 0.302f, 0.188f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.188f, 0.302f, 0.188f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.188f, 0.302f, 0.188f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.188f, 0.302f, 0.188f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.933f, 0.941f, 0.898f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.188f, 0.302f, 0.188f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.188f, 0.302f, 0.188f, 0.85f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.188f, 0.302f, 0.188f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.714f, 0.769f, 0.714f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.714f, 0.769f, 0.714f, 1.0f));
}

std::shared_ptr<ImVec2> Interface::resolutionGet()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return std::make_shared<ImVec2>(static_cast<float>(mode->width), static_cast<float>(mode->height));
}