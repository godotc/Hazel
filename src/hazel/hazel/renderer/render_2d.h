//
// Created by nono on 10/14/23.
//

#ifndef HAZEL_RENDER_2_D_H
#define HAZEL_RENDER_2_D_H

#include "glm/glm.hpp"
#include "orthographic_camera.h"
#include "shader.h"
#include "vertex_array.h"
#include "texture.h"


namespace hazel {


struct Render2dStorage {
    Ref<VertexArray> QuadVertexArray;
    Ref<Shader>      FlatColorShader;
    Ref<Shader>      TextureShader;
};

static Render2dStorage *s_Render2D_Data;

class Render2D
{
  public:
    static void Init();
    static void Shutdown();


    static void BeginScene(const OrthographicsCamera &camera);
    static void EndScene();

    static void DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color);
    static void DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &color);

    static void DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture);
    static void DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture);
};

} // namespace hazel

#endif // HAZEL_RENDER_2_D_H
