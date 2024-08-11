/**
 *  Author: @godot42
 *  Create Time: 2024-07-30 15:23:27
 *  Modified by: @godot42
 *  Modified time: 2024-07-30 16:16:38
 *  Description:
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

    void OnImGuiRender();
};


} // namespace hazel