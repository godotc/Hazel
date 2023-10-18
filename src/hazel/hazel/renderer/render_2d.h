//
// Created by nono on 10/14/23.
//

#ifndef HAZEL_RENDER_2_D_H
#define HAZEL_RENDER_2_D_H

#include "glm/glm.hpp"
#include "orthographic_camera.h"
#include "shader.h"
#include "texture.h"
#include "vertex_array.h"

#include "hazel/__microshit_api_hazel.h"


namespace hazel {


struct Render2dStorage {
    Ref<VertexArray> QuadVertexArray;
    Ref<Shader>      TextureShader;
    Ref<Texture2D>   WhileTexture;
};

static Render2dStorage *s_Render2D_Data;

class HAZEL_API Render2D
{
  public:
    static void Init();
    static void Shutdown();


    static void BeginScene(const OrthographicsCamera &camera);
    static void EndScene();

    // TODO: tint

    static void DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color);
    static void DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &color);

    static void DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));
    static void DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));

    static void DrawRotateQuad(const glm::vec2 &pos, const glm::vec2 &size, float rotation, const glm::vec4 &color);
    static void DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const glm::vec4 &color);

    static void DrawRotateQuad(const glm::vec2 &pos, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));
    static void DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tiling = 1.f, glm::vec4 tint = glm::vec4(1.f));
};

} // namespace hazel

#endif // HAZEL_RENDER_2_D_H
