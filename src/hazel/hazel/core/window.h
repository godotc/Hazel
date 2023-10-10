#pragma once


#include "hazel/event/event.h"
#include "hazel/renderer/graphics_context.h"
#include <any>
#include <type_traits>



namespace hazel {


struct WindowProps {
    using uint = unsigned int;

    std::string Title;
    uint        Width, Height;
    explicit WindowProps(const std::string &title = "Hazel Engine", uint w = 1280, uint h = 720) : Title(title), Width(w), Height(h) {}
};


class HAZEL_API Window
{
  public:
    using EventCallBackFn = std::function<void(Event &)>;

    virtual ~Window() = default;

    virtual void OnUpdate() = 0;

    [[nodiscard]] virtual unsigned int GetWidth() const  = 0;
    [[nodiscard]] virtual unsigned int GetHeight() const = 0;

    virtual void               SetEventCallback(const EventCallBackFn &cb) = 0;
    virtual void               SetVSync(bool bEnable)                      = 0;
    [[nodiscard]] virtual bool IsVSync() const                             = 0;

    [[nodiscard]] virtual std::any GetNativeWindow() const = 0;

    static Window *Create(const WindowProps &props = WindowProps());

  protected:
    GraphicsContext *m_Context;
};

}; // namespace hazel
