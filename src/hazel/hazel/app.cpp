#include "hz_pch.h"

#include "event/event.h"
#include "event/key_event.h"

#include "app.h"


#include "imgui/imgui_layer.h"
#include "input.h"
#include "layer.h"
#include "log.h"

#include "hazel/renderer/render.h"
#include "hazel/renderer/render_command.h"

#include "glm/gtc/type_ptr.hpp"
#include "hazel/key_code.h"


// tmp
#include "GLFW/glfw3.h"

namespace hazel {


App *App::Application = nullptr;

App::App()
{
    HZ_INFO("App construct");
    HZ_CORE_ASSERT(!Application, "Already a application instance");
    Application = this;

    m_Window = Scope<Window>(Window::Create());
    m_Window->SetEventCallback([this](Event &ev) -> void { this->OnEvent(ev); });
    m_Window->SetVSync(true);

    Render::Init();

    m_ImGuiLayer = new ImGuiLayer();
    m_LayerStack.PushOverlay(m_ImGuiLayer);
}

void App::Run()
{
    while (bRunning)
    {
        float    time     = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime   = time;

        if (!bMinimized) {
            for (Layer *layer : m_LayerStack.GetLayers()) {
                layer->OnUpdate(timestep);
            }
        }

        // clang-format off
        m_ImGuiLayer->Begin();
        for (Layer *layer : m_LayerStack.GetLayers()) layer->OnImGuiRender();
        m_ImGuiLayer->End();
        // clang-format on

        m_Window->OnUpdate();
    }

    for (auto layer : m_LayerStack.GetLayers()) {
        layer->OnDetach();
    }
}
void App::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
}

void App::PopLayer(Layer *layer)
{
    m_LayerStack.PophLayer(layer);
}

void App::OnEvent(Event &ev)
{
    if (!ev.IsInCategory(EventCategoryInput)) {
        HZ_CORE_INFO(ev.to_string());
    }

    EventDispatcher dispatcher(ev);
    dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent &ev) -> bool { return OnWindowClose(ev); });
    dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent &ev) -> bool { return OnKeyPressed(ev); });
    dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent &ev) -> bool { return OnWindowResized(ev); });

    auto &Layers = m_LayerStack.GetLayers();
    for (auto it = Layers.end(); it != Layers.begin();) {
        (*--it)->OnEvent(ev);
        if (ev.bHandled)
            break;
    }
}

bool App::OnKeyPressed(KeyPressedEvent &ev)
{
    if (ev.GetKeyCode() == HZ_KEY_ESCAPE) {
        HZ_CORE_WARN("Pressing esc to exit");
        bRunning = false;
    }
    return false;
}

bool App::OnWindowClose(WindowCloseEvent &ev)
{
    bRunning = false;
    return true;
}
bool App::OnWindowResized(WindowResizeEvent &ev)
{
    // NOTICE: on window minimize will be (0, 0)
    if (ev.GetHeight() == 0 || ev.GetHeight() == 0) {
        bMinimized = true;
        return false;
    }

    bMinimized = false;
    Render::OnWindowResized(ev.GetWidth(), ev.GetHeight());

    return false;
}
} // namespace hazel
