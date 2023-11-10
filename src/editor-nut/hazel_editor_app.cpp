#include <hazel.h>

#include "hazel/core/app.h"
#include "hazel/core/entry_point.h"


#include <hazel/imgui/imgui_layer.h>



// entry point

#include "editor_layer.h"



class HazelEditor : public hazel::App
{
  public:
    HazelEditor() : hazel::App("Hazel Editor")
    {
        HZ_INFO("Sandbox construct");
        // PushLayer(new hazel::ExampleLayer);
        PushLayer(new hazel::EditorLayer());
        GetWindow().SetVSync(true);
    }
    ~HazelEditor() override = default;
};


hazel::App *hazel::CreateApplication()
{
    return new HazelEditor;
}
