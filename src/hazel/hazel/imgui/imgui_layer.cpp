#include "hz_pch.h"

#include "hazel/imgui/imgui_layer.h"


#include "GLFW/glfw3.h"
#include "hazel/core/base.h"


#include "imgui.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
// #include "backends/imgui_impl_opengl3_loader.h"

#include "hazel/core/app.h"
#include "hazel/core/log.h"
#include "platform/linux/linux_window.h"


#include "imgui_internal.h"

namespace hazel {

ImGuiLayer::ImGuiLayer() : Layer("ImGuILayer") {}

void ImGuiLayer::OnAttach()
{
    HZ_PROFILE_FUNCTION();

    HZ_CORE_TRACE("{} Attacting...", GetName());

    IMGUI_CHECKVERSION();
    HZ_CORE_INFO("Imgui: v{}", IMGUI_VERSION);



    auto ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx);

    // ctx->DebugLogFlags |= ImGuiDebugLogFlags_EventViewport | ImGuiDebugLogFlags_OutputToTTY;


    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

    io.DisplaySize = ImVec2(1, 1);

    ImGui::StyleColorsDark();

    auto style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        style.WindowRounding              = 0.f;
        style.Colors[ImGuiCol_WindowBg].w = 1.f;
    }

    if (auto *window = dynamic_cast<LinuxWindow *>(&App::Get().GetWindow())) {
        if (auto *native_window = any_cast<GLFWwindow *>(window->GetNativeWindow())) {
            bool bSuccess = ImGui_ImplGlfw_InitForOpenGL(native_window, true);
            HZ_CORE_ASSERT(bSuccess, "imgui glfw backend initialize failed");
        }
    }
    else {
        HZ_CORE_ERROR("imgui glfw backend initialize failed");
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
    HZ_PROFILE_FUNCTION();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::Begin()
{
    HZ_PROFILE_FUNCTION();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    HZ_PROFILE_FUNCTION();

    auto io        = ImGui::GetIO();
    App &app       = App::Get();
    io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere. //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiLayer::OnImGuiRender()
{
    auto       &io  = ImGui::GetIO();
    static bool bOk = true;
    if (ImGui::MenuItem("Viewport", nullptr, io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)) {
        io.ConfigFlags ^= ImGuiConfigFlags_ViewportsEnable;
    }
}

} // namespace hazel
