//
// Created by nono on 10/14/23.
//
#include "hz_pch.h"

#include "debug/instrumentor.h"

#include "glm/fwd.hpp"

#include "render_2d.h"
#include "render_command.h"

#include "buffer.h"
#include "hazel/renderer/shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cstdint>

namespace hazel {


struct QuadVertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 Texcoord;
    // TODO: color, texid
};

struct Render2DData {
    const uint32_t MaxQuads    = 10000;
    const uint32_t MaxVertices = MaxQuads * 4;
    const uint32_t MaxIndices  = MaxQuads * 6;


    Ref<VertexArray>  QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    Ref<Shader>       TextureShader;
    Ref<Texture2D>    WhileTexture;

    uint32_t    QuadIndexCount       = 0;
    QuadVertex *QuadVertexBufferHead = nullptr;
    QuadVertex *QuadVertexBufferPtr  = nullptr;
};

static Render2DData s_Data;


void Render2D::Init()
{
    HZ_PROFILE_FUNCTION();

    s_Data.QuadVertexArray = VertexArray::Create();

    // VB layout
    s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
    s_Data.QuadVertexBuffer->SetLayout({
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float4,    "a_Color"},
        {ShaderDataType::Float2, "a_TexCoord"},
    });
    s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

    // vertex ptrs
    s_Data.QuadVertexBufferHead = new QuadVertex[s_Data.MaxVertices];


    Ref<IndexBuffer> IB;
    {
        uint32_t *Indices = new uint32_t[s_Data.MaxIndices];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6) {
            Indices[i + 0] = offset + 0;
            Indices[i + 1] = offset + 1;
            Indices[i + 2] = offset + 2;

            Indices[i + 3] = offset + 2;
            Indices[i + 4] = offset + 3;
            Indices[i + 5] = offset + 0;

            offset += 4;
        }

        IB = IndexBuffer::Create((uint32_t *)Indices, s_Data.MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(IB);
        delete[] Indices;
    }


    s_Data.WhileTexture = Texture2D::Create(1, 1);
    // R G B A 8bit x 4
    // f == 16 == 2^5
    // 5 * 8 or 4 * 8 orz
    //    uint32_t white_texture_data = 0xffffffff;
    unsigned char white_texture_data[4] = {255, 255, 255, 255};
    s_Data.WhileTexture->SetData(white_texture_data, sizeof(uint32_t));


    s_Data.TextureShader = Shader::Create(FPath("res/shader/texture.glsl"));
    s_Data.TextureShader->Bind();
    s_Data.TextureShader->SetInt("u_Texture", 0);
}

void Render2D::Shutdown()
{
    HZ_PROFILE_FUNCTION();

    delete[] s_Data.QuadVertexBufferHead;
}


void Render2D::BeginScene(const OrthographicsCamera &camera)
{
    HZ_PROFILE_FUNCTION();

    s_Data.TextureShader->Bind();
    s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

    s_Data.QuadIndexCount      = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferHead; // begin
}

void Render2D::EndScene()
{
    HZ_PROFILE_FUNCTION();


    // binary length
    uint32_t size = (uint8_t *)s_Data.QuadVertexBufferPtr - (uint8_t *)s_Data.QuadVertexBufferHead;
    s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferHead, size);

    Flush();
}

void Render2D::Flush()
{
    HZ_PROFILE_SESSION_END();

    s_Data.QuadVertexArray->Bind();
    RenderCommand::DrawIndex(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
}

void Render2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &color)
{
    HZ_PROFILE_FUNCTION();

    s_Data.QuadVertexBufferPtr->Position = pos;
    s_Data.QuadVertexBufferPtr->Color    = color;
    s_Data.QuadVertexBufferPtr->Texcoord = {0.f, 0.f};
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position = pos + glm::vec3{size.x, 0.f, 0.f};
    s_Data.QuadVertexBufferPtr->Color    = color;
    s_Data.QuadVertexBufferPtr->Texcoord = {1.f, 0.f};
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position = pos + glm::vec3{size.x, size.y, 1.f};
    s_Data.QuadVertexBufferPtr->Color    = color;
    s_Data.QuadVertexBufferPtr->Texcoord = {1.f, 1.f};
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position = pos + glm::vec3{0.f, size.y, 1.f};
    s_Data.QuadVertexBufferPtr->Color    = color;
    s_Data.QuadVertexBufferPtr->Texcoord = {0.f, 1.f};
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;

    // auto &shader = s_Data.TextureShader;
    // shader->SetFloat("u_Tiling", 1.f);

    // // bind white texture
    // s_Data.WhileTexture->Bind();

    // glm::mat4 transform(1.f);
    // transform = glm::translate(transform, pos);
    // transform = glm::scale(transform, {size.x, size.y, 1.f});
    // shader->SetMat4("u_Transform", transform);

    // s_Data.QuadVertexArray->Bind();
    // RenderCommand::DrawIndex(s_Data.QuadVertexArray);
}

void Render2D::DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color)
{
    DrawQuad({pos.x, pos.y, 0.f}, size, color);
}

void Render2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tiling, glm::vec4 tint /*= glm::vec4(1.f)*/)
{
    HZ_PROFILE_FUNCTION();

    constexpr auto color = glm::vec4{1.f, 1.f, 1.f, 1.f};

    s_Data.QuadVertexBufferPtr->Position = pos;
    s_Data.QuadVertexBufferPtr->Color    = color;
    s_Data.QuadVertexBufferPtr->Texcoord = {0.f, 0.f};
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position = pos + glm::vec3{size.x, 0.f, 0.f};
    s_Data.QuadVertexBufferPtr->Color    = color;
    s_Data.QuadVertexBufferPtr->Texcoord = {1.f, 0.f};
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position = pos + glm::vec3{size.x, size.y, 1.f};
    s_Data.QuadVertexBufferPtr->Color    = color;
    s_Data.QuadVertexBufferPtr->Texcoord = {1.f, 1.f};
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position = pos + glm::vec3{0.f, size.y, 1.f};
    s_Data.QuadVertexBufferPtr->Color    = color;
    s_Data.QuadVertexBufferPtr->Texcoord = {0.f, 1.f};
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;

    // auto &shader = s_Data.TextureShader;
    // shader->SetFloat4("u_Color", tint); // tint
    // shader->SetFloat("u_Tiling", tiling);

    // texture->Bind();

    // glm::mat4 transform(1.f);
    // transform = glm::translate(transform, pos);
    // transform = glm::scale(transform, {size.x, size.y, 1.f});
    // shader->SetMat4("u_Transform", transform);

    // s_Data.QuadVertexArray->Bind();
    // RenderCommand::DrawIndex(s_Data.QuadVertexArray);
}

void Render2D::DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tiling, glm::vec4 tint /*= glm::vec4(1.f)*/)
{
    DrawQuad({pos.x, pos.y, 0.f}, size, texture, tiling, tint);
}

void Render2D::DrawRotateQuad(const glm::vec2 &pos, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{
    DrawRotateQuad({pos.x, pos.y, 0.f}, size, rotation, color);
}
void Render2D::DrawRotateQuad(const glm::vec2 &pos, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tiling, glm::vec4 tint)
{
    DrawRotateQuad({pos.x, pos.y, 0.f}, size, rotation, texture, tiling, tint);
}


void Render2D::DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tiling, glm::vec4 tint)
{
    HZ_PROFILE_FUNCTION();

    auto &shader = s_Data.TextureShader;
    shader->SetFloat4("u_Color", tint); // tint
    shader->SetFloat("u_Tiling", tiling);

    texture->Bind();

    glm::mat4 transform(1.f);
    transform = glm::translate(transform, pos);
    transform = glm::rotate(transform, rotation, {0, 0, 1});
    transform = glm::scale(transform, {size.x, size.y, 1.f});
    shader->SetMat4("u_Transform", transform);

    s_Data.QuadVertexArray->Bind();
    RenderCommand::DrawIndex(s_Data.QuadVertexArray);
}

void Render2D::DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{
    HZ_PROFILE_FUNCTION();

    auto &shader = s_Data.TextureShader;
    shader->SetFloat4("u_Color", color); // tint
    shader->SetFloat("u_Tiling", 1.f);

    s_Data.WhileTexture->Bind();

    glm::mat4 transform(1.f);
    transform = glm::translate(transform, pos);
    transform = glm::rotate(transform, rotation, {0, 0, 1});
    transform = glm::scale(transform, {size.x, size.y, 1.f});
    shader->SetMat4("u_Transform", transform);

    s_Data.QuadVertexArray->Bind();
    RenderCommand::DrawIndex(s_Data.QuadVertexArray);
}
} // namespace hazel