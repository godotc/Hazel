//
// Created by nono on 9/22/23.
//
#include "hz_pch.h"

#include "glad/glad.h"
#include "opengl_render_api.h"

namespace hazel {
void OpenGLRenderAPI::SetClearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}
void OpenGLRenderAPI::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void OpenGLRenderAPI::DrawIndexed(const std::shared_ptr<VertexArray> &vertex_array)
{
    glDrawElements(GL_TRIANGLES, vertex_array->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}
} // namespace hazel