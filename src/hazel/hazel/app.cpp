#include "event/event.h"
#include "event/key_event.h"
#include "hz_pch.h"

#include "app.h"

#include "hazel.h"

#include "glad/glad.h"
#include "imgui/imgui_layer.h"
#include "input.h"
#include "layer.h"
#include "log.h"
#include <GLFW/glfw3.h>

namespace hazel {

App *App::Application = nullptr;

App::App()
{
    HZ_INFO("App construct");
    HZ_CORE_ASSERT(!Application, "Already a application instance");
    Application = this;

    m_Window     = std::unique_ptr<Window>(Window::Create());
    m_ImGuiLayer = new ImGuiLayer();
    m_LayerStack.PushOverlay(m_ImGuiLayer);

    m_Window->SetEventCallback([this](Event &ev) -> void { this->OnEvent(ev); });
}

App::~App() {}

void App::Run()
{
    while (bRunning) {
        glClearColor(0.3, 0.5, 0.7, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        for (Layer *layer : m_LayerStack.GetLayers()) {
            layer->OnUpdate();
        }

        m_ImGuiLayer->Begin();
        for (Layer *layer : m_LayerStack.GetLayers()) {
            layer->OnImGuiRender();
        }
        m_ImGuiLayer->End();

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

    auto &Layers = m_LayerStack.GetLayers();
    for (auto it = Layers.end(); it != Layers.begin();) {
        (*--it)->OnEvent(ev);
        if (ev.bHandled)
            break;
    }
}

bool App::OnKeyPressed(KeyPressedEvent &ev)
{
    return false;
}

bool App::OnWindowClose(WindowCloseEvent &ev)
{
    bRunning = false;
    return true;
}
} // namespace hazel