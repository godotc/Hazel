#pragma once

#include "__microshit_api_hazel.h"
#include "hazel/core/layer.h"

#include "imgui.h"
#include "utils/delegate.h"
#include <cstddef>



namespace hazel {

extern HAZEL_API ImGuiContext *g_ImguiContext;
#define RESET_IMGUI_CONTEXT() ImGui::SetCurrentContext(hazel::g_ImguiContext);

class HAZEL_API ImGuiLayer : public Layer
{
    bool                            bBlockEvents = true;
    std::map<std::string, ImFont *> m_Fonts;

  public:
    ImGuiLayer();

  public:
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnImGuiRender() override{};
    void         OnEvent(Event &event) override;

    void Begin();
    void End();

    void SetBlockEvents(bool bBlock) { bBlockEvents = bBlock; }
    void SetDarkThemeColors();


    const std::map<std::string, ImFont *> &GetFonts() const { return m_Fonts; };
};

} // namespace hazel
