/**
 *  Author: @godot42
 *  Create Time: 2024-07-30 15:23:39
 *  Modified by: @godot42
 *  Modified time: 2024-07-30 16:58:51
 *  Description:
 */

#include "content_browser_panel.h"
#include "hazel/core/base.h"
#include "hazel/renderer/render_2d.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "utils/path.h"
#include <cstdint>
#include <cwchar>
#include <filesystem>
#include <functional>
#include <string>
#include <system_error>

#include "editor_layer.h"

namespace hazel {



ContentBrowserPanel::ContentBrowserPanel()
{
    m_CurrentDirectory = EditorLayer::DefaultAssetsDirectory() / m_CurrentDirectory;

    m_DirectoryIcon             = Texture2D::Create(FPath("res/texture/editor/folder2.png"));
    m_FileIcon                  = Texture2D::Create(FPath("res/texture/editor/file.png"));
    m_WhiteTexture              = Texture2D::Create(1, 1);
    uint32_t white_texture_data = 0xffffffff;
    m_WhiteTexture->SetData(&white_texture_data, sizeof(uint32_t));
}

void ContentBrowserPanel::OnImGuiRender()
{
    if (!ImGui::Begin("Content Browser")) {
        ImGui::End();
        return;
    }

    // top bar
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    {
        if (m_CurrentDirectory != EditorLayer::DefaultAssetsDirectory()) {
            if (ImGui::Button("<- Back")) {
                if (m_CurrentDirectory.has_parent_path()) {
                    m_CurrentDirectory = m_CurrentDirectory.parent_path();
                }
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle + 1)) {
                if (m_CurrentDirectory.has_parent_path()) {
                    m_CurrentDirectory = m_CurrentDirectory.parent_path();
                }
            }
        }
        ImGui::SameLine();
        std::error_code ec;
        auto            relative_path = std::filesystem::relative(m_CurrentDirectory, EditorLayer::DefaultAssetsDirectory(), ec);
        // HZ_ASSERT(ec == std::errc::d) {} TODO

        ImGui::Text("%s", relative_path.string().c_str());

        // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 30)); // add 10 pixels of padding at the top and bottom
        ImGui::Image((ImTextureID)m_WhiteTexture->GetTextureID(), {ImGui::GetContentRegionAvail().x, 1});
        // ImGui::PopStyleVar();
    }
    ImGui::PopStyleVar();


    static float padding        = 12.0f;
    static float thumbnail_size = 94.0f;

    float cell_size   = thumbnail_size + padding;
    float panel_width = ImGui::GetContentRegionAvail().x;

    int column_count = (int)(panel_width / cell_size);
    if (column_count < 1) {
        column_count = 1;
    }

    ImGui::Columns(column_count, nullptr, false);
    for (const auto &entry : std::filesystem::directory_iterator(m_CurrentDirectory))
    {
        const auto &path          = entry.path();
        const auto &relative_path = std::filesystem::relative(path, EditorLayer::DefaultAssetsDirectory() /*, ec*/);
        const auto  filename_str  = relative_path.filename().string();

        // ImGui::PushID(path.string().c_str());
        ImGui::PushID(filename_str.c_str());

        Ref<Texture2D> icon = entry.is_directory() ? m_DirectoryIcon : m_FileIcon;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0, 0, 0, 0});

        // TODO: make a middleware  to set each uv of texture2d, or needing to adjust by me
        ImGui::ImageButton((ImTextureID)icon->GetTextureID(), {thumbnail_size, thumbnail_size}, {0, 1}, {1, 0});

        // on the context of image button
        if (ImGui::BeginDragDropSource()) {

            if constexpr (std::is_same_v<const wchar_t *, decltype(relative_path.c_str())>)
            {
                auto relative_path_str = relative_path.c_str();
                // relative_path_str[wcslen(relative_path_str) - 1] = L'\0';
                // The payload will be copied
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM",
                                          relative_path_str, (wcslen(relative_path_str) + 1) * sizeof(wchar_t),
                                          ImGuiCond_Once);
            }
            else {
                HZ_CORE_ASSERT(false, "Not wstring in the filesystem path, Unimplemented");
            }
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (entry.is_directory()) {
                m_CurrentDirectory /= entry.path().filename();
            }
        }

        ImGui::TextWrapped("%s", filename_str.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }
    ImGui::Columns(1);

    ImGui::DragFloat("Thumbnail Size", &thumbnail_size, 0.1f, 16.0f, 256.0f);
    ImGui::DragFloat("Padding", &padding, 0.1f, 0.0f, 64.0f);



    ImGui::End();
}

} // namespace hazel