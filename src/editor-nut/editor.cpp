#include <hazel.h>

#include "hazel/core/app.h"
#include "hazel/core/entry_point.h"


#include <hazel/imgui/imgui_layer.h>
#include <wingdi.h>



// entry point

#include "editor_layer.h"
#include "utils/path.h"

namespace hazel {

class HazelEditor : public hazel::App
{
  public:
    HazelEditor(ApplicationSpecification spec)
        : hazel::App(spec)
    {
        HZ_INFO("Editor construct...");
        PushLayer(new hazel::EditorLayer());
        GetWindow().SetVSync(true);


        {
            // this for a exe link to a dll, but dll has static variable
            // and the variable ptr will be null but valid in exe process
            // so we need call these funs to reset it?
            RESET_IMGUI_CONTEXT();
            RESET_IMGUIZMO_CONTEXT();
        }


        HZ_INFO("start arguments:");
        for (int i = 0; i < spec.CommandLineArgs.count; i++) {
            HZ_INFO("{}", spec.CommandLineArgs.args[i]);
        }
    }

    ~HazelEditor() override
    {
        HZ_INFO(" Editor app destruction...");
    }
};



App *CreateApplication(ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec{
        .Name             = "Hazel Editor",
        .WorkingDirectory = FPath("example/project0"),
        .CommandLineArgs  = args,
    };
    return new HazelEditor(spec);
}

}; // namespace hazel
