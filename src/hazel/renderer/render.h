//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_RENDER_H
#define HAZEL_RENDER_H

#include "renderer_api.h"


namespace hazel {

class Shader;
class OrthographicsCamera;

class Render
{
  public:
    static void BeginScene(OrthographicsCamera &Camera);
    static void EndScene();

    //    static void Submit(std::shared_ptr<Shader> &vertex_array);
    static void Submit(const std::shared_ptr<Shader> &shader, std::shared_ptr<VertexArray> &vertex_array, const glm::mat4 transform = glm::mat4(1.f));

  public:
    static inline RendererAPI::API GetRendererAPI()
    {
        return RendererAPI::GetAPI();
    }

  public:
    struct SceneData {
        glm::mat4 ViewProjectionMatrix;
    };

  private:
    static SceneData *m_SceneData;
};

} // namespace hazel

#endif // HAZEL_RENDER_H
