/**
 *  Author: @godot42
 *  Create Time: 2024-07-30 15:23:27
 *  Modified by: @godot42
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-02 20:04:25
 */

#pragma once

#include "hazel/core/base.h"
#include "hazel/renderer/texture.h"
#include <filesystem>


namespace hazel {



class ContentBrowserPanel
{
    std::filesystem::path m_CurrentDirectory;

    Ref<Texture2D> m_DirectoryIcon;
    Ref<Texture2D> m_FileIcon;
    Ref<Texture2D> m_WhiteTexture;

  public:
    ContentBrowserPanel();

    void Init();


    void OnImGuiRender();
};

} // namespace hazel