#include "hz_pch.h"

#include "imgui/imgui_layer.h"

#include "GLFW/glfw3.h"
#include "core.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_opengl3_loader.h"

#include "app.h"
#include "log.h"
#include "platform/linux/linux_window.h"
#include "spdlog/fmt/bundled/ostream.h"



namespace hazel {


namespace imgui = ImGui;

ImGuiLayer::ImGuiLayer() : Layer("ImGuILayer") {}

void ImGuiLayer::OnAttach()
{
    HZ_CORE_TRACE("{} Attacting...", GetName());

    IMGUI_CHECKVERSION();
    HZ_CORE_INFO("Imgui: v{}", IMGUI_VERSION);

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;


    ImGui::StyleColorsDark();

    auto style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        style.WindowRounding              = 0.f;
        style.Colors[ImGuiCol_WindowBg].w = 1.f;
    }

    if (auto *window = static_cast<LinuxWindow *>(&App::Get().GetWindow())) {
        bool bSuccess = ImGui_ImplGlfw_InitForOpenGL(any_cast<GLFWwindow *>(window->GetNativeWindow()), true);
        HZ_CORE_ASSERT(bSuccess, "imgui glfw backend initialize failed");
    }

    // TODO: detect the host opengl version, call imgui init automatically
    // GLint major, minor;
    // glGetIntegerv(GL_MAJOR_VERSION, &major);
    // glGetIntegerv(GL_MINOR_VERSION, &minor);
    // auto glversion = "#version " + std::to_string(major * 100 + minor * 10);
    // HZ_CORE_INFO("Imgui detect the GL version: {}", glversion);

    bool bSuccess = ImGui_ImplOpenGL3_Init("#version 420");
    HZ_CORE_ASSERT(bSuccess, "imgui opengl3 backend initialize failed");
}

void ImGuiLayer::OnDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::Begin()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    auto io        = ImGui::GetIO();
    App &app       = App::Get();
    io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

    static auto time = (float)glfwGetTime();
    io.DeltaTime     = m_Time > 0.f ? (time - m_Time) : (1.f / 60.f);
    m_Time           = time;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere. //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}
void ImGuiLayer::OnImGuiRender()
{
}

} // namespace hazel
