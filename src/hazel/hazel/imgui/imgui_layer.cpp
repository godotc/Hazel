#include "hz_pch.h"

#include "hazel/imgui/imgui_layer.h"


#include "GLFW/glfw3.h"


#include "imgui.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
// #include "backends/imgui_impl_opengl3_loader.h"

#include "hazel/core/app.h"
#include "hazel/core/log.h"
#include "platform/glfw/glfw_window.h"
#include "utils/path.h"

#include <filesystem>
#include <map>
#include <nb30.h>

#include <ImGuizmo.h>



namespace hazel {

ImGuiContext *g_ImguiContext = nullptr;

ImGuiLayer::ImGuiLayer() : Layer("ImGuILayer") {}

void ImGuiLayer::OnAttach()
{
    HZ_PROFILE_FUNCTION();

    HZ_CORE_TRACE("{} Attaching...", GetName());

    IMGUI_CHECKVERSION();
    HZ_CORE_INFO("Imgui: v{}", IMGUI_VERSION);


    g_ImguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(g_ImguiContext);

    // ctx->DebugLogFlags |= ImGuiDebugLogFlags_EventViewport | ImGuiDebugLogFlags_OutputToTTY;


    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#if HAZEL_ENABLE_VIEWPORT
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

    io.DisplaySize = ImVec2(1, 1);



    m_Fonts.insert({"ImGui_Default", io.FontDefault});
    utils::Files::ForeachFileInFolder(
        FPath("res/font/").absolute_path,
        [](auto &path) -> bool {
            return path.string().ends_with(".ttf");
        },
        [&io, &m_Fonts = (this->m_Fonts)](auto &path) -> void {
            ImFont     *font = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 18.f);
            std::string name = utils::Files::GetFileNameWithoutExtension(path.string());
            m_Fonts.insert({name, font});
        });

    io.FontDefault = m_Fonts.find("ImGui_Default")->second;

    // store the default
    // TODO: from deserialized config file
    // m_FontManager.m_Fonts.push({
    //     .name   = "ImGui_Default",
    //     .size   = -1.0f,
    //     .source = "wtf",
    //     .font   = io.FontDefault,
    // });



    ImGui::StyleColorsDark();
    SetDarkThemeColors();

    auto style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        style.WindowRounding              = 0.f;
        style.Colors[ImGuiCol_WindowBg].w = 1.f;
    }

    if (auto *window = dynamic_cast<PlatGLFWWindow *>(&App::Get().GetWindow())) {
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

    ImGuizmo::BeginFrame();
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

void ImGuiLayer::SetDarkThemeColors()
{
    auto &io                  = ImGui::GetIO();
    auto  colors              = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    // Headers
    colors[ImGuiCol_Header]        = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive]  = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button]        = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive]  = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Frame BG
    colors[ImGuiCol_FrameBg]        = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive]  = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab]                = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabHovered]         = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive]          = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused]       = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg]          = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgActive]    = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
}



void ImGuiLayer::OnEvent(Event &event)
{
    if (bBlockEvents) {
        ImGuiIO &io = ImGui::GetIO();
        event.bHandled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
        event.bHandled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
    }
}

} // namespace hazel
