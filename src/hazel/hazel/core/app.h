#pragma once
#include "__microshit_api_hazel.h"
#include "base.h"

#include "hazel/imgui/imgui_layer.h"
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

struct HAZEL_API AppCommandLineArgs {
    int    count = 0;
    char **args  = nullptr;

    const char *operator[](int i) const
    {
        HZ_CORE_ASSERT(i < count);
        return args[i];
    }
};


class HAZEL_API App
{

    AppCommandLineArgs Args;
    LayerStack         m_LayerStack;
    Window            *m_Window;

    ImGuiLayer *m_ImGuiLayer;
    float       m_LastFrameTime = 0.f;

    bool bRunning   = true;
    bool bMinimized = false;

  public:

    using SimpleEvent = MulticastDelegate<>;
    SimpleEvent PreDestruction;
    SimpleEvent PostDestruction;


  public:
    virtual ~App();
    static inline App &Get()
    {
        static App *Application = new App;
        return *Application;
    }

  protected:
    App();
    virtual void Init(const std::string &name = "Hazel Engine", AppCommandLineArgs args = AppCommandLineArgs{});

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

    AppCommandLineArgs GetCommandLineArgs() const { return Args; }

  protected:
    void OnEvent(Event &ev);

    bool OnWindowClose(WindowCloseEvent &ev);
    bool OnWindowResized(WindowResizeEvent &ev);
    bool OnKeyPressed(KeyPressedEvent &ev);
};

App *CreateApplication(AppCommandLineArgs args);


} // namespace hazel
