#pragma once

#include "hz_pch.h"


#include "hazel.h"

struct GLFWwindow;

namespace hazel {


class LinuxWindow : public Window
{
  public:
    explicit LinuxWindow(const WindowProps &props);
    ~LinuxWindow() override;

  public:

    void OnUpdate() override;

    [[nodiscard]] inline unsigned int GetWidth() const override { return m_Data.Width; }
    [[nodiscard]] inline unsigned int GetHeight() const override;

    inline void SetEventCallback(const EventCallBackFn &cb) override { m_Data.EventCallback = cb; }

    void               SetVSync(bool bEnable) override;
    [[nodiscard]] bool IsVSync() const override;

    [[nodiscard]] inline std::any GetNativeWindow() const override { return m_Window; }

  private:
    void Init(const WindowProps &props);
    void ShutDown();

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