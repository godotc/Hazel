#pragma once


#include "hazel/core/log.h"
#include "hazel/event/event.h"
#include "hazel/renderer/graphics_context.h"
#include <any>
#include <type_traits>



namespace hazel {


struct WindowProps {
    using uint32_t = unsigned int;

    std::string Title;
    uint32_t    Width, Height;
    explicit WindowProps(const std::string &title = "Hazel Engine", uint32_t w = 1280, uint32_t h = 720) : Title(title), Width(w), Height(h) {}
};


class HAZEL_API Window
{
  protected:
    GraphicsContext *m_Context;

  public:
    static Window *Create(const WindowProps &props = WindowProps());

  public:
    using EventCallBackFn = std::function<void(Event &)>;

    virtual ~Window()
    {
        HZ_CORE_INFO("Windows context destruction...");
    }

    virtual void OnUpdate() = 0;

    [[nodiscard]] virtual unsigned int GetWidth() const  = 0;
    [[nodiscard]] virtual unsigned int GetHeight() const = 0;

    virtual void               SetEventCallback(const EventCallBackFn &cb) = 0;
    virtual void               SetVSync(bool bEnable)                      = 0;
    [[nodiscard]] virtual bool IsVSync() const                             = 0;

    [[nodiscard]] virtual std::any GetNativeWindow() const = 0;
};

}; // namespace hazel
