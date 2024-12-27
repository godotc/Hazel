#include "hz_pch.h"

#include "hazel/core/layer_stack.h"

#include "hazel/core/base.h"
#include "hazel/core/window.h"


#include "app.h"

#include "hazel/event/application_event.h"
#include "hazel/event/event.h"
#include "hazel/event/key_event.h"
#include "log.h"


#include "hazel/imgui/imgui_layer.h"
#include "layer.h"

#include "hazel/renderer/render.h"
#include "key_code.h"

#include "hazel/renderer/render_2d.h"

// tmp
#include "GLFW/glfw3.h"
#include <filesystem>
#include <ranges>

namespace hazel {


App *App::g_Application = nullptr;

App::App(const ApplicationSpecification& app_sec)
{
    HZ_PROFILE_FUNCTION();

    HZ_INFO("App construct");
    HZ_CORE_ASSERT(!g_Application, "Already a application instance");
    g_Application = this;

    m_ApplicationSpecification = app_sec;
    m_Window = Window::Create(WindowProps(app_sec.Name));
    namespace fs = std::filesystem;
    if (!m_ApplicationSpecification.WorkingDirectory.empty()) {
        HZ_ASSERT(fs::exists(m_ApplicationSpecification.WorkingDirectory), "Working directory does not exist");

        // does this do chdir? https://en.cppreference.com/w/cpp/filesystem/current_path
        std::filesystem::current_path(app_sec.WorkingDirectory);
        HZ_WARN("Set working directory to {}", app_sec.WorkingDirectory.string());
    }



    m_Window->SetEventCallback([this](Event &ev) -> void { this->OnEvent(ev); });
    m_Window->SetVSync(true);

    Render::Init();
    Render2D::Init();

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
}

App::~App()
{
    PostDestruction.Broadcast();
    // the opengl context
    // delete m_Window;
}

void App::Run()
{
    HZ_PROFILE_FUNCTION();

    while (bRunning)
    {
        HZ_PROFILE_SCOPE("Main Loop");

        float    time     = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime   = time;

        if (!bMinimized)
        {
            {
                HZ_PROFILE_SCOPE("LayerStack: All Layers OnUpdate");
                for (Layer *layer : m_LayerStack.GetLayers()) {
                    layer->OnUpdate(timestep);
                }
            }

            {
                m_ImGuiLayer->Begin();
                {
                    HZ_PROFILE_SCOPE("LayerStack: Imgui Render");
                    for (Layer *layer : m_LayerStack.GetLayers()) {
                        layer->OnImGuiRender();
                    }
                }
                m_ImGuiLayer->End();
            }
        }


        m_Window->OnUpdate();
    }

    {
        HZ_PROFILE_SCOPE("LayerStack: all OnDetach");
        for (auto layer : m_LayerStack.GetLayers() | std::views::reverse) {
            HZ_CORE_INFO("Detaching {}...", layer->GetName());
            layer->OnDetach();
        }
    }

    m_LayerStack.Cleanup();
    PreDestruction.Broadcast();
}

void App::Close()
{
    bRunning = false;
}

void App::PushLayer(Layer *layer)
{
    HZ_PROFILE_FUNCTION();
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void App::PopLayer(Layer *layer)
{
    HZ_PROFILE_FUNCTION();
    m_LayerStack.PopLayer(layer);
}

void App::PushOverlay(Layer *layer)
{
    HZ_PROFILE_FUNCTION();
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}
void App::PopOverlay(Layer *layer)
{
    HZ_PROFILE_FUNCTION();
    m_LayerStack.PopOverlay(layer);
}


void App::OnEvent(Event &ev)
{
    HZ_PROFILE_FUNCTION();

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
        if (ev.bHandled) {
            break;
        }
    }
}

bool App::OnKeyPressed(KeyPressedEvent &ev)
{
    if (ev.GetKeyCode() == Key::Escape) {
        HZ_CORE_WARN("Pressing esc to exit");
        bRunning = false;
    }
    return false;
}

bool App::OnWindowClose(WindowCloseEvent &ev)
{
    Close();
    return true;
}
bool App::OnWindowResized(WindowResizeEvent &ev)
{
    HZ_PROFILE_FUNCTION();

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
