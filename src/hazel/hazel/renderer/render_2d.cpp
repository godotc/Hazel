//
// Created by nono on 10/14/23.
//

#include "hz_pch.h"

#include "render_2d.h"
#include "render_command.h"

#include "buffer.h"
#include "platform/opengl/opengl_shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace hazel {



void Render2D::Init()
{
    s_Render2D_Data = new Render2dStorage;

    s_Render2D_Data->QuadVertexArray = VertexArray::Create();

    float vertices[4][3 /*5*/] = {
        {-0.5, -0.5, 0}, // 0, 0},
        { 0.5, -0.5, 0}, // 1, 0},
        {-0.5,  0.5, 0}, // 0, 1},
        { 0.5,  0.5, 0}, // 1, 1},
    };

    uint32_t indices[2][3] = {
        {0, 1, 3},
        {3, 2, 0},
    };

    Ref<VertexBuffer> VB;
    Ref<IndexBuffer>  IB;
    {
        VB.reset(VertexBuffer::Create((float *)vertices, sizeof(vertices)));
        VB->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
        });
        IB.reset(IndexBuffer::Create((uint32_t *)indices, sizeof(indices) / sizeof(uint32_t)));
    }
    s_Render2D_Data->QuadVertexArray->AddVertexBuffer(VB);
    s_Render2D_Data->QuadVertexArray->SetIndexBuffer(IB);

    s_Render2D_Data->FlatColorShader = Shader::Create(FPath("res/shader/flat_color.glsl"));
}

void Render2D::Shutdown()
{
    delete s_Render2D_Data;
}

void Render2D::BeginScene(const OrthographicsCamera &camera)
{
    auto ogl_shader = dynamic_pointer_cast<OpenGLShader>(s_Render2D_Data->FlatColorShader);
    ogl_shader->Bind();
    ogl_shader->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
    ogl_shader->UploadUniformMat4("u_Transform", glm::mat4(1.f));
}

void Render2D::EndScene()
{
}

void Render2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &color)
{
    auto ogl_shader = dynamic_pointer_cast<OpenGLShader>(s_Render2D_Data->FlatColorShader);
    ogl_shader->Bind();
    ogl_shader->UploadUniformFloat4("u_Color", color);
    ogl_shader->UploadUniformMat4("u_Model", glm::translate(glm::mat4(1.f), pos));

    s_Render2D_Data->QuadVertexArray->Bind();
    RenderCommand::DrawIndex(s_Render2D_Data->QuadVertexArray);
}

void Render2D::DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color)
{
    DrawQuad({pos.x, pos.y, 0.f}, size, color);
}
} // namespace hazel