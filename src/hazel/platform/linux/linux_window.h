#pragma once

#include "hz_pch.h"


#include "hazel.h"

struct GLFWwindow;

namespace hazel {


class LinuxWindow : public Window
{
  public:
    explicit LinuxWindow(const WindowProps &props);
    virtual ~LinuxWindow();

  public:

    void OnUpdate() override;

    inline unsigned int GetWidth() const override { return m_Data.Width; }
    inline unsigned int GetHeight() const override { return m_Data.Height; }

    inline void SetEventCallback(const EventCallBackFn &cb) override { m_Data.EventCallback = cb; }

    void SetVSync(bool bEnable) override;
    bool IsVSync() const override;

    GLFWwindow *GetWindow() { return m_Window; }

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