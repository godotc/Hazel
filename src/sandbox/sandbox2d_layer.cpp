//
// Created by nono on 10/11/23.
//

#include "hz_pch.h"

#include "glm/gtc/type_ptr.hpp"
#include "platform/opengl/opengl_shader.h"
#include "sandbox2d_layer.h"


void Sandbox2D::OnAttach()
{
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

    // VA
    m_FlatVA = hazel::VertexArray::Create();
    hazel::Ref<hazel::VertexBuffer> VB;
    hazel::Ref<hazel::IndexBuffer>  IB;
    {
        // VB
        VB.reset(hazel::VertexBuffer::Create((float *)vertices, sizeof(vertices)));
        // Layout
        VB->SetLayout({
            {hazel::ShaderDataType::Float3, "a_Position"},
        });
        // IB
        IB.reset(hazel::IndexBuffer::Create((uint32_t *)indices, sizeof(indices) / sizeof(uint32_t)));
    }
    m_FlatVA->AddVertexBuffer(VB);
    m_FlatVA->SetIndexBuffer(IB);


    //    m_ShaderLibrary.Load("flat_color", FPath("res/shader/flat_color.glsl"));
    m_FlatShader = hazel::Shader::Create(FPath("res/shader/flat_color.glsl"));

    m_Texture     = hazel::Texture2D::Create(FPath("res/texture/face.png"));
    m_ArchTexture = hazel::Texture2D::Create(FPath("res/texture/arch.png"));
}

void Sandbox2D::OnDetach()
{
    Layer::OnDetach();
}

void Sandbox2D::OnUpdate(hazel::Timestep timestep)
{
    m_CameraController.OnUpdate(timestep);

    hazel::RenderCommand::SetClearColor(m_ClearColor);
    hazel::RenderCommand::Clear();

    hazel::Render::BeginScene(m_CameraController.GetCamera());

    m_FlatShader->Bind();
    if (auto ogl_shaer = dynamic_pointer_cast<hazel::OpenGLShader>(m_FlatShader)) {
        ogl_shaer->UploadUniformFloat4("u_Color", m_FlatColor);
    }

    hazel::Render::Submit(m_FlatShader, m_FlatVA, glm::mat4(1.f));

    hazel::Render::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));
    ImGui::ColorEdit4("Flat Color", glm::value_ptr(m_FlatColor));
    ImGui::End();
}
void Sandbox2D::OnEvent(hazel::Event &event)
{
    m_CameraController.OnEvent(event);
}
