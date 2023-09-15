#pragma once

#include "event/event.h"
#include "event/key_event.h"
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
    void Run();

    void OnEvent(Event &ev);
    void PushLayer(Layer *layer);
    void PopLayer(Layer *layer);

  private:
    bool OnWindowClose(WindowCloseEvent &ev);
    bool OnKeyPressed(KeyPressedEvent &ev);


  private:
    std::unique_ptr<Window> m_Window;
    bool                    bRunning = true;

    LayerStack m_LayerStack;
};


App *CreateApplication();


} // namespace hazel
