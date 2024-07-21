#include <hazel.h>

#include "hazel/core/app.h"
#include "hazel/core/entry_point.h"


#include <hazel/imgui/imgui_layer.h>
#include <wingdi.h>



// entry point

#include "editor_layer.h"

namespace hazel {


class HazelEditor : public hazel::App
{
    HazelEditor()
        : App()
    {
        HZ_INFO("Editor construct...");
        PushLayer(new hazel::EditorLayer());
        GetWindow().SetVSync(true);
        RESET_IMGUI_CONTEXT();
        RESET_IMGUIZMO_CONTEXT();
    }


    virtual void Init(const std::string &name, AppCommandLineArgs args) {}

    ~HazelEditor() override
    {
        HZ_INFO(" Editor app destruction...");
    }
};


App *CreateApplication(AppCommandLineArgs args)
{
    HazelEditor::Get();
}

}; // namespace hazel


#ifdef HAZEL_ENTRY_PROGRAM
// /*
extern hazel::App *hazel::CreateApplication(AppCommandLineArgs args);
int                main(int argc, char **argv)
{
    std::cout << "The HZ~ Engine" << std::endl;

    HZ_PROFILE_SESSION_BEGIN("Init", FPath("output/debug/profile/hazel_profile-init.json"));

    hazel::App *app = hazel::CreateApplication({argc, argv});
    HZ_PROFILE_SESSION_END();

    HZ_PROFILE_SESSION_BEGIN("Runtime", FPath("output/debug/profile/hazel_profile-runtime.json"));
    app->Run();
    HZ_PROFILE_SESSION_END();

    HZ_PROFILE_SESSION_BEGIN("ShutDown", FPath("output/debug/profile/hazel_profile-showdown.json"));
    delete app;
    HZ_PROFILE_SESSION_END();

    return 0;
}
// */
#endif