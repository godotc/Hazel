#pragma once

#include "event/event.h"
#include "event/key_event.h"
#include "imgui/imgui_layer.h"
#include "layer_stack.h"
#include "window.h"

#include "event/application_event.h"

namespace hazel {


class App
{
  public:
    App();
    virtual ~App();

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
    std::unique_ptr<Window> m_Window;
    ImGuiLayer             *m_ImGuiLayer;
    LayerStack              m_LayerStack;

    bool bRunning = true;


    static App *Application;
};

App *CreateApplication();


} // namespace hazel
