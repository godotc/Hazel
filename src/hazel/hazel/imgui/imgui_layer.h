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
    bool bBlockEvents = true;

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
};

} // namespace hazel
