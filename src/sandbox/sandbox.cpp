#include <hazel.h>
#include <hazel/imgui/imgui_layer.h>
#include <ostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/matrix.hpp"

namespace hazel {
class NothingLayer : public hazel::Layer
{
  public:
    NothingLayer()
        : Layer("nothing"),
          m_Camera(-1.6, 1.6, -0.9, 0.9)
    {
        float vertices[4][7] = {
            {-0.5, -0.5, 0, 1, 0, 0, 0},
            {0.5, -0.5, 0, 1, 0, 0, 1},
            {-0.5, 0.5, 0, 1, 0, 0, 1},
            {0.5, 0.5, 0, 1, 0, 0},
        };

        uint32_t indices[2][3] = {
            {0, 1, 2},
            {2, 1, 3},
        };

        // VA
        m_VertexArray.reset(VertexArray::Create());
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer>  m_IndexBuffer;
        {
            // VB
            m_VertexBuffer.reset(VertexBuffer::Create((float *)vertices, sizeof(vertices)));
            // Layout
            m_VertexBuffer->SetLayout({
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float4,    "a_Color"},
            });
            // IB
            m_IndexBuffer.reset(IndexBuffer::Create((uint32_t *)indices, sizeof(indices) / sizeof(uint32_t)));
        }
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        // Shader
        std::string vert = R"(
            #version 330 core
            layout (location =0 ) in vec3 a_Position;
            layout (location =1 ) in vec4 a_Color;

            out vec3 pos;
            out vec4 color;

            uniform mat4 u_ViewProjection;

            void main(){
                pos = a_Position;
                color = a_Color;
                gl_Position = u_ViewProjection * vec4(a_Position, 1.f);
            }
        )";
        std::string frag = R"(
            #version 330 core
            in vec3 pos;
            in vec4 color;

            out vec4 out_color;

            void main(){
                out_color = vec4(pos*0.5 + 0.5,1) * color;
            }
        )";
        m_Shader         = std::make_unique<Shader>(vert, frag);
    }

    void OnUpdate() override
    {
        Render::BeginScene(m_Camera);
        m_Camera.SetRotation(m_CameraRotation);
        m_Camera.SetPosition(m_CameraPosition);
        Render::Submit(m_Shader, m_VertexArray);
        Render::EndScene();
        //        exit(-1);

        // clang-format off
        if (hazel::Input::IsKeyPressed(HZ_KEY_A)) m_CameraPosition.x += m_CameraSpeed;
        if (hazel::Input::IsKeyPressed(HZ_KEY_D)) m_CameraPosition.x -= m_CameraSpeed;
        if (hazel::Input::IsKeyPressed(HZ_KEY_W)) m_CameraPosition.y -= m_CameraSpeed;
        if (hazel::Input::IsKeyPressed(HZ_KEY_S)) m_CameraPosition.y += m_CameraSpeed;
        if (hazel::Input::IsKeyPressed(HZ_KEY_Q)) m_CameraRotation -= m_CameraRotateDegree;
        if (hazel::Input::IsKeyPressed(HZ_KEY_E)) m_CameraRotation += m_CameraRotateDegree;
        // clang-format on
    };

    void
    OnImGuiRender() override
    {
        ImGui::Begin("Panel");
        ImGui::SliderFloat("m_CameraRotation", &m_CameraRotation, 0, 360);
        ImGui::SliderFloat3("m_CameraPosition", glm::value_ptr(m_CameraPosition), -1, 1);
        ImGui::End();
    }

    void OnEvent(hazel::Event &event) override
    {
        hazel::EventDispatcher dispatcher(event);
    }

  private:
  private:
    float     m_CameraRotation     = 0.f;
    glm::vec3 m_CameraPosition     = glm::vec3(0.f);
    float     m_CameraSpeed        = 0.02f;
    float     m_CameraRotateDegree = 1.f;

    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<Shader>      m_Shader;
    OrthographicsCamera          m_Camera;
};


} // namespace hazel
class Sandbox : public hazel::App
{
  public:
    Sandbox()
    {
        HZ_INFO("Sandbox construct");
        PushLayer(new hazel::NothingLayer);
    }
    ~Sandbox() override
    {
    }
};


hazel::App *hazel::CreateApplication()
{
    return new Sandbox;
}
