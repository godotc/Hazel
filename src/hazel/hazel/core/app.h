#pragma once
#include "base.h"

#include "hazel/imgui/imgui_layer.h"
#include "hazel/renderer/framebuffer.h"
#include "imgui.h"
#include "layer_stack.h"
#include "utils/delegate.h"
#include "window.h"

#include "hazel/event/application_event.h"
#include "hazel/event/event.h"
#include "hazel/event/key_event.h"


#include "hazel/renderer/buffer.h"
#include "hazel/renderer/shader.h"
#include "hazel/renderer/vertex_array.h"

#include "hazel/core/timestep.h"
#include "hazel/renderer/orthographic_camera.h"

namespace hazel {



class HAZEL_API App
{
    static App *Application;

    LayerStack m_LayerStack;
    Window    *m_Window;

    ImGuiLayer *m_ImGuiLayer;
    float       m_LastFrameTime = 0.f;

    bool bRunning   = true;
    bool bMinimized = false;

  public:

    using SimpleEvent = MulticastDelegate<>;
    SimpleEvent PreDestruction;
    SimpleEvent PostDestruction;


  public:
    App(const std::string &name = "Hazel Engine");
    virtual ~App();

    static inline App &Get() { return *Application; }

  public:
    void Run();
    void Close();

  public:
    void PushLayer(Layer *layer);
    void PopLayer(Layer *layer);
    void PushOverlay(Layer *layer);
    void PopOverlay(Layer *layer);

    ImGuiLayer *GetImGuiLayer() { return m_ImGuiLayer; }

    inline Window &GetWindow() { return *m_Window; }

  protected:
    void OnEvent(Event &ev);

    bool OnWindowClose(WindowCloseEvent &ev);
    bool OnWindowResized(WindowResizeEvent &ev);
    bool OnKeyPressed(KeyPressedEvent &ev);
};

App *CreateApplication();


} // namespace hazel
