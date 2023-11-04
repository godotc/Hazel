#pragma once


#include "hazel/core/base.h"

#include "hazel/core/window.h"
#include <any>

struct GLFWwindow;

namespace hazel {

class LinuxWindow : public Window
{
  public:
    explicit LinuxWindow(const WindowProps &props);
    ~LinuxWindow() override;

  public:
    void OnUpdate() override;

    inline void SetEventCallback(const EventCallBackFn &cb) override
    {
        m_Data.EventCallback = cb;
    }

  public:
    [[nodiscard]] inline std::any     GetNativeWindow() const override { return m_Window; }
    [[nodiscard]] inline unsigned int GetWidth() const override { return m_Data.Width; }
    [[nodiscard]] inline unsigned int GetHeight() const override;

    void               SetVSync(bool bEnable) override;
    [[nodiscard]] bool IsVSync() const override;

  private:
    void Init(const WindowProps &props);
    void ShutDown();

  private:
    void initCallbacks();

  private:
    struct WindowData {
        std::string     Title;
        unsigned int    Width, Height;
        EventCallBackFn EventCallback;
        bool            bVSync;
    };

    WindowData m_Data;

    GLFWwindow *m_Window;
};

}; // namespace hazel
