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

#include <cstddef>
#include <filesystem>
#include <imgui.h>
//
#include <ImGuizmo.h>
#include <optional>
#include <queue>
#include <unordered_map>
#include <utility>



namespace hazel {

extern HAZEL_API ImGuiContext *g_ImguiContext;
#define RESET_IMGUI_CONTEXT() ImGui::SetCurrentContext(hazel::g_ImguiContext);
#define RESET_IMGUIZMO_CONTEXT() ImGuizmo::SetImGuiContext(hazel::g_ImguiContext);


struct FontSpec {
    std::string           name;
    float                 size;
    std::filesystem::path source;
    ImFont               *font;

    bool operator==(const FontSpec &other) const
    {
        return name == other.name && size == other.size && source == other.source && font == other.font;
    }
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

struct FontManager {
    friend class ImGuiLayer;
    LRUCache<FontSpec> m_Fonts = LRUCache<FontSpec>(5);

    const FontSpec &GetCurrentFontSpec() const { return m_Fonts.top(); }

    // TODO right ref
    void ChangeFont(FontSpec &&fontSpec)
    {
        auto &io = ImGui::GetIO();

        auto font = m_Fonts.find(fontSpec);
        if (font.has_value()) {
            io.FontDefault = font->font;
            return;
        }

        io.FontDefault = io.Fonts->AddFontFromFileTTF(fontSpec.source.string().c_str(), fontSpec.size);
        fontSpec.font  = io.FontDefault;
        m_Fonts.push(fontSpec);
    }
};


class HAZEL_API ImGuiLayer : public Layer
{
    bool                            bBlockEvents = true;
    std::map<std::string, ImFont *> m_Fonts;
    // FontManager m_FontManager;

  public:
    ImGuiLayer();

  public:
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnImGuiRender() override {};
    void         OnEvent(Event &event) override;

    void Begin();
    void End();

    void SetBlockEvents(bool bBlock) { bBlockEvents = bBlock; }
    void SetDarkThemeColors();

    const char *DefaultFontName() { return "ImGui_Default"; };

    // FontManager GetFontManager() const { return m_FontManager; }
    const std::map<std::string, ImFont *> &GetFonts() const { return m_Fonts; };
};

} // namespace hazel
