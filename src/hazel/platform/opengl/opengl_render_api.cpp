//
// Created by nono on 9/22/23.
//
#include "hz_pch.h"


#include "glad/glad.h"
#include "hazel/renderer/buffer.h"
#include "opengl_render_api.h"

namespace hazel {

void OpenGLRenderAPI::Init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLRenderAPI::SetClearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}
void OpenGLRenderAPI::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void OpenGLRenderAPI::DrawIndexed(const Ref<VertexArray> &vertex_array)
{
    glDrawElements(GL_TRIANGLES, vertex_array->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}

} // namespace hazel