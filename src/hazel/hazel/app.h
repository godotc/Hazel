#pragma once
#include "hazel/core.h"

#include "event/event.h"
#include "event/key_event.h"
#include "imgui/imgui_layer.h"
#include "layer_stack.h"
#include "window.h"

#include "event/application_event.h"
#include "hazel/renderer/buffer.h"
#include "hazel/renderer/vertex_array.h"
#include "renderer/shader.h"

#include "hazel/core/timestep.h"
#include "renderer/orthographic_camera.h"

namespace hazel {


class HAZEL_API App
{
  public:
    App();
    virtual ~App() = default;

  public:
    static inline App &Get() { return *Application; }

  public:
    void Run();

  public:
    void PushLayer(Layer *layer);
    void PopLayer(Layer *layer);

    inline Window &GetWindow() { return *m_Window; }

  protected:
    void OnEvent(Event &ev);

    bool OnWindowClose(WindowCloseEvent &ev);
    bool OnKeyPressed(KeyPressedEvent &ev);


  private:
    Scope<Window> m_Window;
    ImGuiLayer             *m_ImGuiLayer;
    LayerStack              m_LayerStack;
    float                   m_LastFrameTime = 0.f;

    bool bRunning = true;

  private:
    static App *Application;
};

App *CreateApplication();


} // namespace hazel
