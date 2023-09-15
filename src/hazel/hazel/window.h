#pragma once


#include "event/event.h"
#include <type_traits>


namespace hazel {



struct WindowProps {
    using uint = unsigned int;

    std::string Title;
    uint        Width, Height;

    WindowProps(const std::string &title = "Hazel Engine", uint w = 1280, uint h = 720) : Title(title), Width(w), Height(h) {}
};



class HAZEL_API Window
{
  public:
    using EventCallBackFn = std::function<void(Event &)>;

    virtual ~Window() {}

    virtual void OnUpdate() = 0;

    virtual unsigned int GetWidth() const  = 0;
    virtual unsigned int GetHeight() const = 0;

    virtual void SetEventCallback(const EventCallBackFn &cb) = 0;
    virtual void SetVSync(bool bEnable)                      = 0;
    virtual bool IsVSync() const                             = 0;

    static Window *Create(const WindowProps &props = WindowProps());
};

}; // namespace hazel
