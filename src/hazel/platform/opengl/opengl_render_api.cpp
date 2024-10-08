//
// Created by nono on 9/22/23.
//
#include "hz_pch.h"

#include "glad/glad.h"
#include "hazel/renderer/buffer.h"
#include "opengl_render_api.h"
#include <cstdint>

namespace hazel {


void OpenGLRenderAPI::Init()
{
    HZ_PROFILE_FUNCTION();

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);

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

void OpenGLRenderAPI::DrawIndexed(const Ref<VertexArray> &vertex_array, uint32_t index_count)
{
    vertex_array->Bind();
    uint32_t count = index_count ? index_count : vertex_array->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, (GLsizei)count, GL_UNSIGNED_INT, nullptr);

    // FIXME:
    // glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLRenderAPI::DrawLines(const Ref<VertexArray> &vertex_array, uint32_t vertex_count)
{
    vertex_array->Bind();
    glDrawArrays(GL_LINES, 0, (GLsizei)vertex_count);
}

void OpenGLRenderAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    glViewport(x, y, w, h);
}

void OpenGLRenderAPI::SetLineWidth(float width)
{
    glLineWidth(width);
}

} // namespace hazel