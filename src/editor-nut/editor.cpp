#include <hazel.h>

#include "hazel/core/app.h"
#include "hazel/core/entry_point.h"


#include <hazel/imgui/imgui_layer.h>



// entry point

#include "editor_layer.h"

namespace hazel {
using hazel::App;


class HazelEditor : public hazel::App
{
  public:
    HazelEditor() : hazel::App("Hazel Editor")
    {
        HZ_INFO("Editor construct...");
        PushLayer(new hazel::EditorLayer());
        GetWindow().SetVSync(true);
        RESET_IMGUI_CONTEXT();
    }

    ~HazelEditor() override
    {
        HZ_INFO(" Editor app destruction...");
    }
};


App *CreateApplication()
{
    return new HazelEditor;
}

}; // namespace hazel