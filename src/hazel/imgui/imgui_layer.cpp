#include "imgui/imgui_layer.h"
#include "event/event.h"
#include "hz_pch.h"

#include "GLFW/glfw3.h"
#include "core.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "app.h"
#include "log.h"
#include "platform/linux/linux_window.h"

// TODO: temp shit
#include "glad/glad.h"

namespace hazel {


namespace imgui = ImGui;

ImGuiLayer::ImGuiLayer() : Layer("ImGuILayer")
{
}

void ImGuiLayer::OnAttach()
{
    HZ_CORE_TRACE("{} Attacting...", GetName());

    IMGUI_CHECKVERSION();
    HZ_CORE_INFO("Imgui: v{}", IMGUI_VERSION);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // deperacted
    // io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;

    auto style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        style.Colors[ImGuiCol_WindowBg].w = 1.f;
    }

    if (auto *window = dynamic_cast<LinuxWindow *>(&App::Get().GetWindow())) {
        bool bSuccess = ImGui_ImplGlfw_InitForOpenGL(any_cast<GLFWwindow *>(window->GetNativeWindow()), true);
        HZ_CORE_ASSERT(bSuccess, "imgui glfw backend initialize failed");
    }
    bool bSuccess = ImGui_ImplOpenGL3_Init("#version 460");
    HZ_CORE_ASSERT(bSuccess, "imgui opengl3 backend initialize failed");
}

void ImGuiLayer::OnDetach()
{
    ImGui::DestroyContext();
}

void ImGuiLayer::OnUpdate()
{
    auto io        = ImGui::GetIO();
    App &app       = App::Get();
    io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

    static auto time = (float)glfwGetTime();
    io.DeltaTime     = m_Time > 0.f ? (time - m_Time) : (1.f / 60.f);
    m_Time           = time;

    {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        static bool bShow = true;
        ImGui::ShowDemoWindow(&bShow);

        ImGui::Begin("hello world");
        ImGui::Text("heeeeeee");
        ImGui::End();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
}

void ImGuiLayer::OnEvent(Event &event)
{
    if (!event.IsInCategory(EventCategoryInput)) {
        HZ_CORE_TRACE("ImGuiLayer::OnEvent ({})", event.to_string());
    }
    EventDispatcher dispatcher(event);

    dispatcher.Dispatch<KeyTypedEvent>(HZ_BIND_EVENT(this, &ImGuiLayer::OnKeyTypedEvent));
    dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT(this, &ImGuiLayer::OnKeyPressedEvent));
    dispatcher.Dispatch<KeyReleasedEvent>(HZ_BIND_EVENT(this, &ImGuiLayer::OnKeyReleasedEvent));

    dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT(this, &ImGuiLayer::OnMouseButtonPressedEvent));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(HZ_BIND_EVENT(this, &ImGuiLayer::OnMouseButtonReleasedEvent));
    dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT(this, &ImGuiLayer::OnMouseScrolledEvent));

    dispatcher.Dispatch<MouseMoveEvent>(HZ_BIND_EVENT(this, &ImGuiLayer::OnMouseMoveEvent));

    dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT(this, &ImGuiLayer::OnWindowResizedEvent));
}
bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent &ev)
{
    auto &io = imgui::GetIO();

    if (ev.GetKeyCode() > 0 && ev.GetKeyCode() < 0x10000) {
        io.AddInputCharacter((unsigned int)ev.GetKeyCode());
    }

    return true;
}


bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent &ev)
{
    auto &io                          = imgui::GetIO();
    io.MouseDown[ev.GetMouseButton()] = true;
    return false;
}
bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent &ev)
{
    auto &io                          = imgui::GetIO();
    io.MouseDown[ev.GetMouseButton()] = false;
    return false;
}

bool ImGuiLayer::OnMouseMoveEvent(MouseMoveEvent &ev)
{
    auto &io    = imgui::GetIO();
    io.MousePos = ImVec2(ev.GetX(), ev.GetY());
    return false;
}
bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent &ev)
{
    auto &io = imgui::GetIO();
    io.MouseWheelH += ev.GetOffsetX();
    io.MouseWheel += ev.GetOffsetY();
    return false;
}
bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent &ev)
{
    auto &io = imgui::GetIO();

    io.KeysDown[ev.GetKeyCode()] = true;

    io.KeyCtrl  = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyAlt   = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
    return false;
}
bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent &ev)
{
    auto &io                     = imgui::GetIO();
    io.KeysDown[ev.GetKeyCode()] = false;
    return false;
}
bool ImGuiLayer::OnWindowResizedEvent(WindowResizeEvent &ev)
{
    auto &io                   = imgui::GetIO();
    io.DisplaySize             = ImVec2(ev.GetWidth(), ev.GetHeight());
    io.DisplayFramebufferScale = ImVec2(1.f, 1.f);
    glViewport(0, 0, ev.GetWidth(), ev.GetHeight());

    return false;
}

} // namespace hazel