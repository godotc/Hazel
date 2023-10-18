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
    HZ_PROFILE_FUNCTION();

    s_Render2D_Data                  = new Render2dStorage;
    s_Render2D_Data->QuadVertexArray = VertexArray::Create();

    float vertices[4][5] = {
        {-0.5, -0.5, 0, 0, 0},
        { 0.5, -0.5, 0, 1, 0},
        {-0.5,  0.5, 0, 0, 1},
        { 0.5,  0.5, 0, 1, 1},
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
            {ShaderDataType::Float3,     "a_Position"},
            {ShaderDataType::Float2, "a_TextureCoord"},
        });
        IB.reset(IndexBuffer::Create((uint32_t *)indices, sizeof(indices) / sizeof(uint32_t)));
    }
    s_Render2D_Data->QuadVertexArray->AddVertexBuffer(VB);
    s_Render2D_Data->QuadVertexArray->SetIndexBuffer(IB);

    s_Render2D_Data->WhileTexture = Texture2D::Create(1, 1);
    // R G B A 8bit x 4
    // f == 16 == 2^5
    // 5 * 8 or 4 * 8 orz
    //    uint32_t white_texture_data = 0xffffffff;
    unsigned char white_texture_data[4] = {255, 255, 255, 255};
    s_Render2D_Data->WhileTexture->SetData(white_texture_data, sizeof(uint32_t));


    s_Render2D_Data->TextureShader = Shader::Create(FPath("res/shader/texture.glsl"));
    s_Render2D_Data->TextureShader->Bind();
    s_Render2D_Data->TextureShader->SetInt("u_Texture", 0);
}

void Render2D::Shutdown()
{
    HZ_PROFILE_FUNCTION();
    delete s_Render2D_Data;
}

void Render2D::BeginScene(const OrthographicsCamera &camera)
{
    HZ_PROFILE_FUNCTION();

    s_Render2D_Data->TextureShader->Bind();
    s_Render2D_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
}

void Render2D::EndScene()
{
    HZ_PROFILE_FUNCTION();
}

void Render2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &color)
{
    HZ_PROFILE_FUNCTION();

    auto &shader = s_Render2D_Data->TextureShader;
    shader->SetFloat4("u_Color", color);

    // bind white texture
    s_Render2D_Data->WhileTexture->Bind(0);

    glm::mat4 transform(1.f);
    transform = glm::translate(transform, pos);
    transform = glm::scale(transform, {size.x, size.y, 1.f});
    shader->SetMat4("u_Transform", transform);

    s_Render2D_Data->QuadVertexArray->Bind();
    RenderCommand::DrawIndex(s_Render2D_Data->QuadVertexArray);
}

void Render2D::DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color)
{

    DrawQuad({pos.x, pos.y, 0.f}, size, color);
}

void Render2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture)
{
    HZ_PROFILE_FUNCTION();

    auto &shader = s_Render2D_Data->TextureShader;
    shader->SetFloat4("u_Color", {1, 1, 1, 1}); // tint
    texture->Bind();

    glm::mat4 transform(1.f);
    transform = glm::translate(transform, pos);
    transform = glm::scale(transform, {size.x, size.y, 1.f});
    shader->SetMat4("u_Transform", transform);

    s_Render2D_Data->QuadVertexArray->Bind();
    RenderCommand::DrawIndex(s_Render2D_Data->QuadVertexArray);
}
void Render2D::DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture)
{
    DrawQuad({pos.x, pos.y, 0.f}, size, texture);
}
} // namespace hazel