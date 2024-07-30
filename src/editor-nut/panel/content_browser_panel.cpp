/**
 *  Author: @godot42
 *  Create Time: 2024-07-30 15:23:39
 *  Modified by: @godot42
 *  Modified time: 2024-07-30 16:58:51
 *  Description:
 */

#include "content_browser_panel.h"
#include "imgui.h"
#include "utils/path.h"
#include <filesystem>
#include <system_error>

namespace hazel {

// update it when have project
static std::filesystem::path GetDefaultAssetsDirectory()
{
    static std::filesystem::path s_AssetsDirectory = FPath("res");
    return s_AssetsDirectory;
}



ContentBrowserPanel::ContentBrowserPanel()
{
    m_CurrentDirectory = GetDefaultAssetsDirectory() / m_CurrentDirectory;
}

void ContentBrowserPanel::OnImGuiRender()
{
    if (!ImGui::Begin("Content Browser")) {
        ImGui::End();
        return;
    }
    if (m_CurrentDirectory != GetDefaultAssetsDirectory()) {
        if (ImGui::Button("<- Back")) {
            if (m_CurrentDirectory.has_parent_path()) {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }
    }
    ImGui::SameLine();
    std::error_code ec;
    auto            relative_path = std::filesystem::relative(m_CurrentDirectory, GetDefaultAssetsDirectory(), ec);
    // HZ_ASSERT(ec == std::errc::d) {}

    ImGui::Text("%s", relative_path.string().c_str());


    for (const auto &entry : std::filesystem::directory_iterator(m_CurrentDirectory))
    {
        const auto &path          = entry.path();
        const auto &relative_path = std::filesystem::relative(path, GetDefaultAssetsDirectory() /*, ec*/);

        const auto filename_str = relative_path.filename().string();

        if (ImGui::Button(filename_str.c_str()))
        {
            if (entry.is_directory()) {
                m_CurrentDirectory /= entry.path().filename();
            }
        }
    }

    ImGui::End();
}

} // namespace hazel