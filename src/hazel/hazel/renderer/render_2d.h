//
// Created by nono on 10/14/23.
//

#ifndef HAZEL_RENDER_2_D_H
#define HAZEL_RENDER_2_D_H

#include "hazel/renderer/subtexture_2d.h"
#include "orthographic_camera.h"
#include "texture.h"

#include "__microshit_api_hazel.h"
#include <cinttypes>


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
    static void Init();
    static void Shutdown();


    static void BeginScene(const OrthographicsCamera &camera);
    static void EndScene();
    static void Flush();


    static void DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color);
    static void DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &color);

    static void DrawQuad(const glm::mat4 &transf, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));

    static void DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));
    static void DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));

    static void DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));
    static void DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));


    /**
     * @brief
     *
     * @param pos
     * @param size
     * @param rotation by radian
     * @param color
     */
    static void DrawRotateQuad(const glm::vec2 &pos, const glm::vec2 &size, float rotation, const glm::vec4 &color);
    static void DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const glm::vec4 &color);

    static void DrawRotateQuad(const glm::vec2 &pos, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));
    static void DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));

    static void DrawRotateQuad(const glm::vec2 &pos, const glm::vec2 &size, float rotation, const Ref<SubTexture2D> &subtexture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));
    static void DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<SubTexture2D> &subtexture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));


    static Statistics &GetStatics();
    static void        ResetStats();


  private:
    static void FlushAndReset();
};

} // namespace hazel

#endif // HAZEL_RENDER_2_D_H
