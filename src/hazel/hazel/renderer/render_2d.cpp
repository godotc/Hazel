//
// Created by nono on 10/14/23.
//
#include "hz_pch.h"


#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/trigonometric.hpp"

#include "hazel/debug/instrumentor.h"

#include "render_2d.h"
#include "render_command.h"

#include "buffer.h"
#include "hazel/renderer/shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "hazel/renderer/texture.h"

namespace hazel {


struct QuadVertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 Texcoord;
    float     TextureIndex;
    float     TilingFactor;

    // TODO: color, texid
};

struct Render2DData {
    static const uint32_t MaxQuads        = 10000;
    static const uint32_t MaxVertices     = MaxQuads * 4;
    static const uint32_t MaxIndices      = MaxQuads * 6;
    static const uint32_t MaxTextureSlots = 32;


    Ref<VertexArray>  QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    Ref<Shader>       TextureShader;
    Ref<Texture2D>    WhileTexture;

    uint32_t                                    QuadIndexCount       = 0;
    QuadVertex                                 *QuadVertexBufferHead = nullptr;
    QuadVertex                                 *QuadVertexBufferPtr  = nullptr;
    std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
    uint32_t                                    TextureSlotIndex = 1; // 0 => white texture


    std::array<glm::vec4, 4> QuadVertexPositions;
};

static Render2DData s_Data;


void Render2D::Init()
{
    HZ_PROFILE_FUNCTION();

    // VertexArray
    s_Data.QuadVertexArray = VertexArray::Create();
    {
        // VB layout
        s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
        s_Data.QuadVertexBuffer->SetLayout({
            {ShaderDataType::Float3,     "a_Position"},
            {ShaderDataType::Float4,        "a_Color"},
            {ShaderDataType::Float2,     "a_TexCoord"},
            { ShaderDataType::Float,     "a_TexIndex"},
            { ShaderDataType::Float, "a_TilingFactor"},
        });
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

        // vertex ptrs
        s_Data.QuadVertexBufferHead = new QuadVertex[s_Data.MaxVertices];


        // Index buffer
        Ref<IndexBuffer> IB;
        uint32_t        *Indices = new uint32_t[s_Data.MaxIndices];
        {
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
        }
        s_Data.QuadVertexArray->SetIndexBuffer(IB);
        delete[] Indices;
    }

    // basic quad data
    s_Data.QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.f};
    s_Data.QuadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.f};
    s_Data.QuadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.f};
    s_Data.QuadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.f};


    // white texture
    {
        s_Data.WhileTexture = Texture2D::Create(1, 1);
        // R G B A 8bit x 4
        // f == 16 == 2^5
        // 5 * 8 or 4 * 8 orz
        //    uint32_t white_texture_data = 0xffffffff;
        unsigned char white_texture_data[4] = {255, 255, 255, 255};
        s_Data.WhileTexture->SetData(white_texture_data, sizeof(uint32_t));

        // init texture slots
        s_Data.TextureSlots[0] = s_Data.WhileTexture;
    }

    // sampler (texture slots)
    int32_t samplers[s_Data.MaxTextureSlots];
    for (uint32_t i = 0; i < s_Data.MaxTextureSlots; ++i) {
        samplers[i] = i;
    }

    // shader
    s_Data.TextureShader = Shader::Create(FPath("res/shader/texture.glsl"));
    s_Data.TextureShader->Bind();

    // s_Data.TextureShader->SetInt("u_Texture", 0);
    s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);
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

    s_Data.TextureSlotIndex = 1;
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

    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i) {
        s_Data.TextureSlots[i]->Bind(i);
    }
    RenderCommand::DrawIndex(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
}

void Render2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &color)
{
    HZ_PROFILE_FUNCTION();

    const float texture_index = 0.f; // white texture
    const float tiling        = 1.f; // white texture

    glm::mat4 transform = glm::translate(glm::mat4(1.f), pos) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});


    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[0];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {0.f, 0.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[1];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {1.f, 0.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[2];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {1.f, 1.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[3];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {0.f, 1.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;
}

void Render2D::DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color)
{
    DrawQuad({pos.x, pos.y, 0.f}, size, color);
}

void Render2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tiling, glm::vec4 tint /*= glm::vec4(1.f)*/)
{
    HZ_PROFILE_FUNCTION();

    float texture_index = 0.f;
    {

        // find already exit or not
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i) {
            if (*s_Data.TextureSlots[i].get() == *texture.get()) {
                texture_index = (float)i;
                break;
            }
        }

        if (texture_index == 0.f)
        {
            texture_index                                = (float)s_Data.TextureSlotIndex; // default 1
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            ++s_Data.TextureSlotIndex;
        }
    }

    constexpr auto color = glm::vec4{1.f, 1.f, 1.f, 1.f};

    glm::mat4 transform = glm::translate(glm::mat4(1.f), pos) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});


    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[0];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {0.f, 0.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[1];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {1.f, 0.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[2];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {1.f, 1.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[3];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {0.f, 1.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;
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

    const auto &color = tint;

    float texture_index = 0.f;
    {

        // find already exit or not
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i) {
            if (*s_Data.TextureSlots[i].get() == *texture.get()) {
                texture_index = (float)i;
                break;
            }
        }

        if (texture_index == 0.f)
        {
            texture_index                                = (float)s_Data.TextureSlotIndex; // default 1
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            ++s_Data.TextureSlotIndex;
        }
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.f), pos) *
                          glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.f, 0.f, 1.f}) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});


    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[0];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {0.f, 0.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[1];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {1.f, 0.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[2];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {1.f, 1.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[3];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {0.f, 1.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;
}

void Render2D::DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{
    HZ_PROFILE_FUNCTION();

    const float tiling        = 1.f;
    const float texture_index = 0.f;

    glm::mat4 transform = glm::translate(glm::mat4(1.f), pos) *
                          glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.f, 0.f, 1.f}) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});


    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[0];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {0.f, 0.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[1];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {1.f, 0.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[2];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {1.f, 1.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[3];
    s_Data.QuadVertexBufferPtr->Color        = color;
    s_Data.QuadVertexBufferPtr->Texcoord     = {0.f, 1.f};
    s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
    s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;
}
} // namespace hazel