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
    HazelEditor(AppCommandLineArgs args)
        : hazel::App("Hazel Editor", args)
    {
        HZ_INFO("Editor construct...");
        PushLayer(new hazel::EditorLayer());
        GetWindow().SetVSync(true);
        RESET_IMGUI_CONTEXT();
        RESET_IMGUIZMO_CONTEXT();
    }

    ~HazelEditor() override
    {
        HZ_INFO(" Editor app destruction...");
    }
};


App *CreateApplication(AppCommandLineArgs args)
{
    return new HazelEditor(args);
}

}; // namespace hazel