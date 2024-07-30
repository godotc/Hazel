/**
 *  Author: @godot42
 *  Create Time: 2024-07-30 15:23:27
 *  Modified by: @godot42
 *  Modified time: 2024-07-30 16:16:38
 *  Description:
 */


#include <filesystem>


namespace hazel {



class ContentBrowserPanel
{
    std::filesystem::path m_CurrentDirectory;

  public:
    ContentBrowserPanel();

    void OnImGuiRender();
};


} // namespace hazel