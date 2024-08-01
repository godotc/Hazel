/**
 * auto_header Author: @godot42
 * auto_header Create Time: 2024-07-28 20:32:18
 * auto_header Modified by: @godot42
 * auto_header Modified time: 2024-07-30 18:51:31
 * auto_header Description:
 */

#pragma once

#include "__microshit_api_hazel.h"
#include "hazel/core/layer.h"
#include "hazel/core/log.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <imgui.h>
//
#include <ImGuizmo.h>
#include <memory>
#include <optional>



namespace hazel {

extern HAZEL_API ImGuiContext *g_ImguiContext;
#define RESET_IMGUI_CONTEXT() ImGui::SetCurrentContext(hazel::g_ImguiContext);
#define RESET_IMGUIZMO_CONTEXT() ImGuizmo::SetImGuiContext(hazel::g_ImguiContext);


struct FontSpec {

    struct FontData {
        void    *font_data;
        uint32_t font_data_size;
        FontData(void *font_data, uint32_t font_data_size) : font_data(font_data), font_data_size(font_data_size) {}
        ~FontData() { IM_FREE(font_data); }
    };

    std::string               name;
    float                     pixel_size;
    std::filesystem::path     file_path;
    std::shared_ptr<FontData> font_data;

    bool operator==(const FontSpec &other) const { return name == other.name && pixel_size == other.pixel_size && file_path == other.file_path && font_data == other.font_data; }
};



template <class T>
class LRUCache
{
    size_t       m_Capacity;
    size_t       m_Size;
    std::list<T> m_Values;

  public:

    LRUCache(size_t capacity) : m_Capacity(capacity), m_Size(0) { m_Values.resize(capacity); }

    const T &top() const { return m_Values.front(); }

    void push(const T &val)
    {
        m_Values.push_front(val);
        ++m_Size;
        if (m_Size > m_Capacity) {
            m_Values.pop_back();
            --m_Size;
        }
    }
    void clear()
    {
        auto &io = ImGui::GetIO();
    }

    std::optional<T> find(const T &val)
    {
        for (const auto &v : m_Values)
        {
            if (v == val) {
                return {v};
            }
        }
        return {};
    }
};

struct ImGuiLayer;

// struct FontManager {
//     friend class ImGuiLayer;

//     std::optional<FontSpec> m_PendingFont = std::nullopt;
//     LRUCache<FontSpec>      m_Fonts       = LRUCache<FontSpec>(5);

//     const FontSpec &GetCurrentFontSpec() const { return m_Fonts.top(); }


//     FontManager() = default;

//     // disable copy
//     FontManager(const FontManager &other)            = delete;
//     FontManager &operator=(const FontManager &other) = delete;
//     FontManager(FontManager &&other)                 = delete;

//     // TODO right ref
//     void ChangeFont(const FontSpec &font_spec)
//     {
//         auto &io = ImGui::GetIO();

//         auto font = m_Fonts.find(font_spec);
//         if (font.has_value()) {
//             m_PendingFont.emplace(font.value());
//             return;
//         }

//         m_PendingFont.emplace(font_spec);
//         // HZ_CORE_ERROR("{}", m_PendingFont->name);
//         assert(m_PendingFont.has_value());
//     }

//     void RealChangeFont()
//     {
//         // return if there is no pending font changing
//         if (!m_PendingFont.has_value()) {
//             return;
//         }
//         auto &io = ImGui::GetIO();


//         auto font_spec = m_PendingFont.value();

//         // load font from file if there is no font data
//         if (!font_spec.font_data)
//         {
//             size_t size;
//             auto   data         = ImFileLoadToMemory(font_spec.file_path.string().c_str(), "rb", &size);
//             font_spec.font_data = std::make_shared<FontSpec::FontData>(data, size);
//             m_Fonts.push(font_spec);
//         }

//         ImFontConfig config;
//         config.FontDataOwnedByAtlas = false;
//         ImFont *font                = io.Fonts->AddFontFromMemoryTTF(font_spec.font_data->font_data,
//                                                                      font_spec.font_data->font_data_size,
//                                                                      font_spec.pixel_size,
//                                                                      &config);

//         // ImFontAtlas
//         // io.Fonts->Build();
//         // ImGui_ImplOpenGL3_DestroyFontsTexture();
//         ImGui_ImplOpenGL3_CreateFontsTexture();
//         // ImGui::GetCurrentContext()->Font->ContainerAtlas->Build();
//         ImGui::SetCurrentFont(font);
//         m_PendingFont.reset();
//     }
// };


class HAZEL_API ImGuiLayer : public Layer
{
    bool                            bBlockEvents = true;
    std::map<std::string, ImFont *> m_Fonts;
    // FontManager m_FontManager;

  public:
    ImGuiLayer();

  public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(Timestep timestep) override;
    void OnImGuiRender() override {};
    void OnEvent(Event &event) override;

    void Begin();
    void End();

    void SetBlockEvents(bool bBlock) { bBlockEvents = bBlock; }
    void SetDarkThemeColors();

    const char *DefaultFontName() { return "ImGui_Default"; };

    // FontManager GetFontManager() const { return m_FontManager; }
    const std::map<std::string, ImFont *> &GetFonts() const { return m_Fonts; };
};

} // namespace hazel
