/**
 * @ Author: godot42
 * @ Create Time: 2024-07-28 20:32:18
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-15 01:56:54
 * @ Description:
 */

//
// Created by nono on 10/14/23.
//
#include "hz_pch.h"
//
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"
#include "hazel/renderer/uniform_buffer.h"

#include "render_2d.h"

#include "hazel/core/app.h"

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "hazel/core/base.h"


#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float4.hpp"

#include "hazel/debug/instrumentor.h"

#include "platform/opengl/gl_macros.h"
#include "render_command.h"

#include "buffer.h"
#include "hazel/renderer/shader.h"

#include "hazel/renderer/texture.h"
#include <array>
#include <cstddef>
#include <cstdint>

#include "utils/path.h"

#include "hazel/scene/component.h"


namespace hazel {


struct QuadVertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 Texcoord;
    float     TextureIndex;
    float     TilingFactor;
    uint32_t  EntityId = -1; // Editor only? will not you pick it in game?
};

struct CircleVertex {
    glm::vec3 WorldPosition;
    glm::vec3 LocalPostion;
    glm::vec4 Color;
    float     Thickness;
    float     Fade;

    uint32_t EntityId = -1; // Editor only? will not you pick it in game?
};

struct LineVertex {
    glm::vec3 Position;
    glm::vec4 Color;

    uint32_t EntityId = -1; // Editor only? will not you pick it in game?
};

struct Render2DData {
    static const uint32_t MaxQuads        = 1000;
    static const uint32_t MaxVertices     = MaxQuads * 4;
    static const uint32_t MaxIndices      = MaxQuads * 6;
    static const uint32_t MaxTextureSlots = 32;


    // TODO: let them as material
    // quad
    Ref<VertexArray>  QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    Ref<Shader>       QuadShader;

    const std::array<glm::vec4, 4> QuadVertexPositions = {
        glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
        glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
        glm::vec4(0.5f, 0.5f, 0.0f, 1.0f),
        glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f),
    };

    uint32_t    QuadIndexCount       = 0;
    QuadVertex *QuadVertexBufferHead = nullptr;
    QuadVertex *QuadVertexBufferPtr  = nullptr;


    /* Quad White/default texture*/
    Ref<Texture2D>           QuadWhiteTexture;
    std::array<glm::vec2, 4> QuadWhiteTextureCoord;
    glm::vec2               *DefaultQuadTextureCoord = nullptr; // just the alias of WhiteTextureCoord

    /* Quad textures*/
    std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
    uint32_t                                    TextureSlotIndex = 1; // 0 => white texture

    //----------

    // circle
    Ref<VertexArray>  CircleVertexArray;
    Ref<VertexBuffer> CircleVertexBuffer;
    Ref<Shader>       CircleShader;

    uint32_t      CircleIndexCount       = 0;
    CircleVertex *CircleVertexBufferHead = nullptr;
    CircleVertex *CircleVertexBufferPtr  = nullptr;
    // ---------

    // line
    Ref<VertexArray>  LineVertexArray;
    Ref<VertexBuffer> LineVertexBuffer;
    Ref<Shader>       LineShader;

    uint32_t    LineVertexCount      = 0;
    LineVertex *LineVertexBufferHead = nullptr;
    LineVertex *LineVertexBufferPtr  = nullptr;

    // 0 ~ 1.F?
    float LineWidth = 1.0f;
    // ---------



    // Camera
    struct CameraData {
        glm::mat4 ViewProjection;
    } CameraBuffer;
    Ref<UniformBuffer> CameraUniformBuffer;


    Render2D::Statistics Stats;

} s_Data; // namespace hazel

Render2D::Statistics &Render2D::GetStatics()
{
    return s_Data.Stats;
}


void Render2D::ResetStats()
{
    memset(&s_Data.Stats, 0, sizeof(Statistics));
}



void Render2D::CleanupRender2D()
{
    // Reason: White texture hold by s_Data and s_Data's TextureSlots, so release here will not work
    s_Data.QuadWhiteTexture.reset();
    for (auto &texture : s_Data.TextureSlots) {
        texture.reset();
    }
    // s_Data.TextureSlots.at(0) = nullptr;
}

void Render2D::Init()
{
    HZ_PROFILE_FUNCTION();

    App::Get().PreDestruction.Add(Render2D::CleanupRender2D);


    // Quads
    {
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
                {   ShaderDataType::Int,     "a_EntityId"},
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
        // s_Data.QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.f};
        // s_Data.QuadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.f};
        // s_Data.QuadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.f};
        // s_Data.QuadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.f};

        s_Data.QuadWhiteTextureCoord[0] = {0.f, 0.f};
        s_Data.QuadWhiteTextureCoord[1] = {1.f, 0.f};
        s_Data.QuadWhiteTextureCoord[2] = {1.f, 1.f};
        s_Data.QuadWhiteTextureCoord[3] = {0.f, 1.f};

        s_Data.DefaultQuadTextureCoord = s_Data.QuadWhiteTextureCoord.data();



        // quad white texture
        {
            s_Data.QuadWhiteTexture = Texture2D::Create(1, 1);
            // R G B A 8bit x 4
            // f == 16 == 2^5
            // 5 * 8 or 4 * 8 orz
            //    uint32_t white_texture_data = 0xffffffff;
            unsigned char white_texture_data[4] = {254, 254, 254, 254};
            s_Data.QuadWhiteTexture->SetData(white_texture_data, sizeof(uint32_t));

            // init texture slots
            s_Data.TextureSlots[0] = s_Data.QuadWhiteTexture;
        }

        // sampler (texture slots)
        int32_t samplers[s_Data.MaxTextureSlots];
        for (uint32_t i = 0; i < s_Data.MaxTextureSlots; ++i) {
            samplers[i] = i;
        }

        // shader
        s_Data.QuadShader = Shader::Create(FPath("res/shader/render2d_quad.glsl"));
    }
    // s_Data.TextureShader->Bind();
    // s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);


    // Circles
    {
        s_Data.CircleVertexArray = VertexArray::Create();
        {
            s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
            s_Data.CircleVertexBuffer->SetLayout({
                {ShaderDataType::Float3, "a_WorldPosition"},
                {ShaderDataType::Float3, "a_LocalPosition"},
                {ShaderDataType::Float4,         "a_Color"},
                { ShaderDataType::Float,     "a_Thickness"},
                { ShaderDataType::Float,          "a_Fade"},
                {   ShaderDataType::Int,      "a_EntityId"},
            });
            s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
            s_Data.CircleVertexBufferHead = new CircleVertex[s_Data.MaxVertices];
            s_Data.CircleVertexArray->SetIndexBuffer(s_Data.QuadVertexArray->GetIndexBuffer());
        }

        s_Data.CircleShader = Shader::Create(FPath("res/shader/render2d_circle.glsl"));
    }

    // Lines
    {
        s_Data.LineVertexArray = VertexArray::Create();
        {
            s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
            s_Data.LineVertexBuffer->SetLayout({
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float4,    "a_Color"},
                {   ShaderDataType::Int, "a_EntityId"},
            });
            s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
            s_Data.LineVertexBufferHead = new LineVertex[s_Data.MaxVertices];
            // s_Data.LineVertexArray->SetIndexBuffer(s_Data.QuadVertexArray->GetIndexBuffer());
        }

        s_Data.LineShader = Shader::Create(FPath("res/shader/render2d_line.glsl"));
    }



    // the uniform buffers
    s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Render2DData::CameraData), 0);

} // namespace hazel

void Render2D::Shutdown()
{
    HZ_PROFILE_FUNCTION();

    delete[] s_Data.QuadVertexBufferHead;
}

void Render2D::BeginScene(const Camera &camera, const glm::mat4 &transform)
{
    HZ_PROFILE_FUNCTION();

    glm::mat4 view_projection = camera.GetProjection() * glm::inverse(transform);

    // just set camera once for one scene process
    s_Data.CameraBuffer.ViewProjection = std::move(view_projection);
    s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Render2DData::CameraData));

    StartBatch();
}


void Render2D::BeginScene(const EditorCamera &camera)
{
    HZ_PROFILE_FUNCTION();

    glm::mat4 view_projection = camera.GetViewProjection();
    // just set camera once for one scene process
    s_Data.CameraBuffer.ViewProjection = std::move(view_projection);
    s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Render2DData::CameraData));

    StartBatch();
}



[[deprecated]]
void Render2D::BeginScene(const OrthographicCamera &camera)
{
    HZ_PROFILE_FUNCTION();

    s_Data.QuadShader->Bind();
    s_Data.QuadShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

    StartBatch();
}

void Render2D::StartBatch()
{
    s_Data.QuadIndexCount      = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferHead; // begin

    s_Data.CircleIndexCount      = 0;
    s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferHead; // begin

    s_Data.LineVertexCount     = 0;
    s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferHead; // begin

    s_Data.TextureSlotIndex = 1;
}


void Render2D::FlushAndReset()
{
    HZ_PROFILE_FUNCTION();

    EndScene();

    s_Data.QuadIndexCount      = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferHead; // begin

    s_Data.CircleIndexCount      = 0;
    s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferHead; // begin

    s_Data.LineVertexCount     = 0;
    s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferHead; // begin


    s_Data.TextureSlotIndex = 1;
}

void Render2D::EndScene()
{
    HZ_PROFILE_FUNCTION();


    Flush();
}

void Render2D::Flush()
{
    HZ_PROFILE_SESSION_END();
    if (s_Data.QuadIndexCount) {

        // Bind textures
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i) {
            s_Data.TextureSlots[i]->Bind(i);
        }

        // binary length
        uint32_t size = (uint8_t *)s_Data.QuadVertexBufferPtr - (uint8_t *)s_Data.QuadVertexBufferHead;
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferHead, size);

        // WHY bind at flush?
        s_Data.QuadShader->Bind();
        RenderCommand::DrawIndex(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
        ++s_Data.Stats.DrawCalls;
    }

    if (s_Data.CircleIndexCount)
    {

        // binary length
        uint32_t size = (uint8_t *)s_Data.CircleVertexBufferPtr - (uint8_t *)s_Data.CircleVertexBufferHead;
        s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferHead, size);

        s_Data.CircleShader->Bind();
        RenderCommand::DrawIndex(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
        ++s_Data.Stats.DrawCalls;
    }

    if (s_Data.LineVertexCount) {

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        uint32_t size = (uint8_t *)s_Data.LineVertexBufferPtr - (uint8_t *)s_Data.LineVertexBufferHead;
        s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferHead, size);

        s_Data.LineShader->Bind();
        RenderCommand::SetLineWidth(s_Data.LineWidth);
        RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
        ++s_Data.Stats.DrawCalls;
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


void Render2D::DrawSprite(const glm::mat4 &transf, const SpriteRendererComponent &src, int entity_id)
{
    if (src.Texture) {
        DrawQuad(transf, src.Texture, src.TilingFactor, src.Color, entity_id);
    }
    else {
        DrawQuad(transf, src.Color, entity_id);
    }
}

void Render2D::DrawCircle(const glm::mat4 &transf, const glm::vec4 &color, float thickness, float fade, int entity_id)
{
    HZ_PROFILE_FUNCTION();

    // TODO: implement for circle
    if (s_Data.CircleIndexCount >= Render2DData::MaxIndices) {
        FlushAndReset();
    }

    constexpr size_t quad_vertex_count = 4;

    const auto &transform = transf;

    for (size_t i = 0; i < quad_vertex_count; ++i) {
        s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
        s_Data.CircleVertexBufferPtr->LocalPostion  = s_Data.QuadVertexPositions[i] * 2.f;
        s_Data.CircleVertexBufferPtr->Color         = color;
        s_Data.CircleVertexBufferPtr->Thickness     = thickness;
        s_Data.CircleVertexBufferPtr->Fade          = fade;
        s_Data.CircleVertexBufferPtr->EntityId      = entity_id;
        s_Data.CircleVertexBufferPtr++;
    }
    s_Data.CircleIndexCount += 6;

    ++s_Data.Stats.QuadCount;
}

void Render2D::DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec4 &color, int entity_id)
{
    if (s_Data.LineVertexCount >= Render2DData::MaxIndices) {
        FlushAndReset();
    }

    s_Data.LineVertexBufferPtr->Position = start;
    s_Data.LineVertexBufferPtr->Color    = color;
    s_Data.LineVertexBufferPtr->EntityId = entity_id;
    ++s_Data.LineVertexBufferPtr;
    ++s_Data.LineVertexCount;

    s_Data.LineVertexBufferPtr->Position = end;
    s_Data.LineVertexBufferPtr->Color    = color;
    s_Data.LineVertexBufferPtr->EntityId = entity_id;
    ++s_Data.LineVertexBufferPtr;
    ++s_Data.LineVertexCount;

    s_Data.LineVertexCount += 2;
}

void Render2D::DrawRect(const glm::vec3 &postion, const glm::vec2 &size, const glm::vec4 &color, int entity_id)
{
    float     half_w = size.x * 0.5f;
    float     half_h = size.y * 0.5f;
    glm::vec3 p0     = glm::vec3(postion.x - half_w, postion.y - half_h, postion.z);
    glm::vec3 p1     = glm::vec3(postion.x + half_w, postion.y - half_h, postion.z);
    glm::vec3 p2     = glm::vec3(postion.x + half_w, postion.y + half_h, postion.z);
    glm::vec3 p3     = glm::vec3(postion.x - half_w, postion.y + half_h, postion.z);

    DrawLine(p0, p1, color);
    DrawLine(p1, p2, color);
    DrawLine(p2, p3, color);
    DrawLine(p3, p0, color);
}

void Render2D::DrawRect(const glm::mat4 &transf, const glm::vec4 &color, int entity_id)
{
    glm::vec3     line_vertices[4];
    constexpr int quad_vertex_count = 4;
    for (size_t i = 0; i < quad_vertex_count; ++i) {
        line_vertices[i] = transf * s_Data.QuadVertexPositions[i];
    }

    // quad ok but line not works
    // DrawQuad(transf, color, entity_id);

    DrawLine(line_vertices[0], line_vertices[1], color);
    DrawLine(line_vertices[1], line_vertices[2], color);
    DrawLine(line_vertices[2], line_vertices[3], color);
    DrawLine(line_vertices[3], line_vertices[0], color);
}



void Render2D::DrawQuad(const glm::mat4 &transf, float tiling, const glm::vec4 &tint)
{
    HZ_PROFILE_FUNCTION();

    if (s_Data.QuadIndexCount >= Render2DData::MaxIndices) {
        FlushAndReset();
    }

    constexpr size_t quad_vertex_count = 4;
    const auto      &color             = tint;
    const float      texture_index     = 0.f;

    const auto &transform = transf;

    for (size_t i = 0; i < quad_vertex_count; ++i) {
        s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color        = color;
        s_Data.QuadVertexBufferPtr->Texcoord     = s_Data.QuadWhiteTextureCoord[i];
        s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
        s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
        s_Data.QuadVertexBufferPtr++;
    }
    s_Data.QuadIndexCount += 6;
    ++s_Data.Stats.QuadCount;
}

void Render2D::DrawQuad(const glm::mat4 &transf, const glm::vec4 &tint, int entity_id)
{
    HZ_PROFILE_FUNCTION();

    if (s_Data.QuadIndexCount >= Render2DData::MaxIndices) {
        FlushAndReset();
    }

    constexpr size_t quad_vertex_count = 4;
    const auto      &color             = tint;
    const float      texture_index     = 0.f;
    const float      tiling            = 1.f;

    const auto &transform = transf;

    for (size_t i = 0; i < quad_vertex_count; ++i) {
        s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color        = color;
        s_Data.QuadVertexBufferPtr->Texcoord     = s_Data.QuadWhiteTextureCoord[i];
        s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
        s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
        s_Data.QuadVertexBufferPtr->EntityId     = entity_id;
        s_Data.QuadVertexBufferPtr++;
    }
    s_Data.QuadIndexCount += 6;
    ++s_Data.Stats.QuadCount;
}

void Render2D::DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, float tiling, const glm::vec4 &tint, int entity_id)
{
    HZ_PROFILE_FUNCTION();

    if (s_Data.QuadIndexCount >= Render2DData::MaxIndices) {
        FlushAndReset();
    }

    constexpr size_t quad_vertex_count = 4;
    const auto      &color             = tint;
    float            texture_index     = 0.f;
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


    for (size_t i = 0; i < quad_vertex_count; ++i) {
        s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color        = color;
        s_Data.QuadVertexBufferPtr->Texcoord     = s_Data.QuadWhiteTextureCoord[i];
        s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
        s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
        s_Data.QuadVertexBufferPtr->EntityId     = entity_id;
        s_Data.QuadVertexBufferPtr++;
    }
    s_Data.QuadIndexCount += 6;
    ++s_Data.Stats.QuadCount;
}

void Render2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<Texture2D> &texture, float tiling, const glm::vec4 &tint)
{
    HZ_PROFILE_FUNCTION();
    if (s_Data.QuadIndexCount >= Render2DData::MaxIndices) {
        FlushAndReset();
    }
    const auto &color    = tint;
    const float rotation = 0.f;

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
                          glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});


    for (int i = 0; i < 4; ++i) {
        s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color        = color;
        s_Data.QuadVertexBufferPtr->Texcoord     = s_Data.DefaultQuadTextureCoord[i];
        s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
        s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;
    ++s_Data.Stats.QuadCount;
}

void Render2D::DrawQuad(const glm::vec3 &pos, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, float tiling, const glm::vec4 &tint)
{
    HZ_PROFILE_FUNCTION();

    constexpr size_t     quad_vertex_count = 4;
    const auto          &color             = tint;
    const glm::vec2     *texture_coords    = subtexture->GetTexcoord();
    const Ref<Texture2D> texture           = subtexture->GetTexture();
    const float          rotation          = 0.f;

    if (s_Data.QuadIndexCount >= Render2DData::MaxIndices)
    {
        FlushAndReset();
    }

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
                          glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});


    for (size_t i = 0; i < quad_vertex_count; ++i) {
        s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color        = color;
        s_Data.QuadVertexBufferPtr->Texcoord     = texture_coords[i];
        s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
        s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
        s_Data.QuadVertexBufferPtr++;
    }
    s_Data.QuadIndexCount += 6;
    ++s_Data.Stats.QuadCount;
}

void Render2D::DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const glm::vec4 &tint)
{
    HZ_PROFILE_FUNCTION();
    if (s_Data.QuadIndexCount >= Render2DData::MaxIndices) {
        FlushAndReset();
    }
    const auto &color         = tint;
    const float texture_index = 0.f;
    const float tiling        = 1.f;

    glm::mat4 transform = glm::translate(glm::mat4(1.f), pos) *
                          glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});


    for (int i = 0; i < 4; ++i) {
        s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color        = color;
        s_Data.QuadVertexBufferPtr->Texcoord     = s_Data.DefaultQuadTextureCoord[i];
        s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
        s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
        s_Data.QuadVertexBufferPtr->EntityId     = -1;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;
    ++s_Data.Stats.QuadCount;
}
void Render2D::DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tiling, const glm::vec4 &tint)
{
    HZ_PROFILE_FUNCTION();
    if (s_Data.QuadIndexCount >= Render2DData::MaxIndices) {
        FlushAndReset();
    }
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
                          glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});


    for (int i = 0; i < 4; ++i) {
        s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color        = color;
        s_Data.QuadVertexBufferPtr->Texcoord     = s_Data.DefaultQuadTextureCoord[i];
        s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
        s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
        s_Data.QuadVertexBufferPtr->EntityId     = -1;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;
    ++s_Data.Stats.QuadCount;
}

void Render2D::DrawRotateQuad(const glm::vec3 &pos, const glm::vec2 &size, float rotation, const Ref<SubTexture2D> &subtexture, float tiling, const glm::vec4 &tint)
{
    HZ_PROFILE_FUNCTION();

    constexpr size_t     quad_vertex_count = 4;
    const auto          &color             = tint;
    const glm::vec2     *texture_coords    = subtexture->GetTexcoord();
    const Ref<Texture2D> texture           = subtexture->GetTexture();

    if (s_Data.QuadIndexCount >= Render2DData::MaxIndices)
    {
        FlushAndReset();
    }

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
                          glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});


    for (size_t i = 0; i < quad_vertex_count; ++i) {
        s_Data.QuadVertexBufferPtr->Position     = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color        = color;
        s_Data.QuadVertexBufferPtr->Texcoord     = texture_coords[i];
        s_Data.QuadVertexBufferPtr->TextureIndex = texture_index;
        s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
        s_Data.QuadVertexBufferPtr->EntityId     = -1;
        s_Data.QuadVertexBufferPtr++;
    }
    s_Data.QuadIndexCount += 6;
    ++s_Data.Stats.QuadCount;
}

float Render2D::GetLineWidth()
{
    return s_Data.LineWidth;
}

void Render2D::SetLineWidth(float width)
{
    s_Data.LineWidth = width;
}



} // namespace hazel