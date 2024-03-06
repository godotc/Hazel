#include "hz_pch.h"

#include "hazel/core/app.h"
#include "hazel/utils/platform_utils.h"


#include <any>
#include <commdlg.h>
#include <glfw/glfw3.h>
#include <minwinbase.h>
#include <string>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

namespace hazel {

std::string FileDialogs::OpenFile(const char *filter)
{
    OPENFILENAMEA ofn;
    CHAR          file_size[260] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);

    ofn.hwndOwner = glfwGetWin32Window(std::any_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow()));

    ofn.lpstrFile    = file_size;
    ofn.nMaxFile     = sizeof(file_size);
    ofn.lpstrFilter  = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    return {};
}

std::string FileDialogs::SaveFile(const char *filter)
{
    OPENFILENAMEA ofn;
    CHAR          file_size[260] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);

    ofn.hwndOwner = glfwGetWin32Window(std::any_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow()));

    ofn.lpstrFile    = file_size;
    ofn.nMaxFile     = sizeof(file_size);
    ofn.lpstrFilter  = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetSaveFileNameA(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    return {};
}

} // namespace hazel