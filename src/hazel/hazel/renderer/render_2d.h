/**
 * @ Author: godot42
 * @ Create Time: 2024-07-28 20:32:18
 * @ Modified by: @godot42
 * @ Modified time: 2024-11-13 20:55:52
 * @ Description:
 */

//
// Created by nono on 10/14/23.
//

#ifndef HAZEL_RENDER_2_D_H
#define HAZEL_RENDER_2_D_H

#include "glm/fwd.hpp"
#include "hazel/renderer/camera.h"
#include "hazel/renderer/subtexture_2d.h"
#include "hazel/scene/editor_camera.h"
#include "hazel/scene/scene.h"
#include "orthographic_camera.h"
#include "texture.h"

#include "__microshit_api_hazel.h"
#include <cstdint>


namespace hazel {



class HAZEL_API Render2D
{
  public:
    struct Statistics {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;
        uint32_t GetTotalVertexCount() { return QuadCount * 4; }
        uint32_t GetTotalIndexCount() { return QuadCount * 6; }
    };


  public:
    static Statistics &GetStatics();
    static void        ResetStats();

    static void Init();
    static void Shutdown();
    static void CleanupRender2D();


    static void BeginScene(const EditorCamera &camera);
    static void BeginScene(const Camera &camera, const glm::mat4 &transform);
    static void BeginScene(const OrthographicCamera &camera); // TOD: remove

    static void StartBatch();
    static void EndScene();
    static void Flush();

    static void DrawSprite(const glm::mat4 &transf, const SpriteRendererComponent &src, int entity_id = -1);

    static void DrawCircle(const glm::mat4 &transf, const glm::vec4 &color, float thickness = 1.f, float fade = 0.005f, int entity_id = -1);

    static void DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec4 &color, int entity_id = -1);

    static void DrawRect(const glm::vec3 &postion, const glm::vec2 &size, const glm::vec4 &color, int entity_id = -1);
    static void DrawRect(const glm::mat4 &transf, const glm::vec4 &color, int entity_id = -1);


    static void DrawQuad(const glm::mat4 &transf, float tiling = 1.f, const glm::vec4 &tint = glm::vec4(1.f));
    static void DrawQuad(const glm::mat4 &transf, const glm::vec4 &tint = glm::vec4(1.f), int entity_id = -1);
    static void DrawQuad(const glm::mat4 &transf, const Ref<Texture2D> &texture, float tiling = 1.f, const glm::vec4 &tint = glm::vec4(1.f), int entity_id = -1);

    static void DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tiling = 1.f, const glm::vec4 &tint = glm::vec4(1.f));
    static void DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, float tiling = 1.f, const glm::vec4 &tint = glm::vec4(1.f));

    static void DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation = 0.f, const glm::vec4 &tint = glm::vec4(1.f));
    static void DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tiling = 1.f, const glm::vec4 &tint = glm::vec4(1, 1, 1, 1));
    static void DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<SubTexture2D> &subtexture, float tiling = 1.f, const glm::vec4 &tint = glm::vec4(1, 1, 1, 1));

  public:
    static float GetLineWidth();
    static void  SetLineWidth(float width);

  private:
    static void FlushAndReset();
};

} // namespace hazel

#endif // HAZEL_RENDER_2_D_H
