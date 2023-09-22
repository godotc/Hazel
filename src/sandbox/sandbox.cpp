#include <hazel.h>
#include <hazel/imgui/imgui_layer.h>
#include <ostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/matrix.hpp"

#include "platform/opengl/opengl_shader.h"

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
            uniform mat4 u_Transform;

            void main(){
                pos = a_Position;
                color = a_Color;
                gl_Position = u_ViewProjection *  u_Transform * vec4(a_Position, 1.f);
            }
        )";
        std::string frag = R"(
            #version 330 core
            in vec3 pos;
            in vec4 color;

            uniform vec4  u_Color;

            out vec4 out_color;

            void main(){
                out_color =  u_Color;
                            //vec4(pos*0.5 + 0.5,1) * color;
            }
        )";
        m_Shader.reset(hazel::Shader::Create(vert, frag));
    }

    void OnUpdate(Timestep ts) override
    {
        float dt = ts;

        //        HZ_TRACE("Delta time : {}s {}ms ", ts.GetSeconds(), ts.GetMiliseconds());

        RenderCommand::SetClearColor({0.3, 0.5, 0.7, 1});
        RenderCommand::Clear();

        Render::BeginScene(m_Camera);
        m_Camera.SetRotation(m_CameraRotation);
        m_Camera.SetPosition(m_CameraPosition);

        glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));

        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
            {
                glm::vec3 pos(i * 0.11f, j * 0.11f, 0.f);
                pos *= m_SquarePosition;

                glm::vec4 color(sin(i), cos(i), tan(i), 1);

                static_pointer_cast<OpenGLShader>(m_Shader)->UploadUniformFloat4("u_Color", color);
                glm::mat4 tranform = glm::translate(glm::mat4(1.f), pos) * scale;

                Render::Submit(m_Shader, m_VertexArray, tranform);
            }
        Render::EndScene();

        // clang-format off
        if (hazel::Input::IsKeyPressed(HZ_KEY_A)) m_CameraPosition.x -= m_CameraSpeed * dt;
        if (hazel::Input::IsKeyPressed(HZ_KEY_D)) m_CameraPosition.x += m_CameraSpeed * dt;
        if (hazel::Input::IsKeyPressed(HZ_KEY_W)) m_CameraPosition.y += m_CameraSpeed * dt;
        if (hazel::Input::IsKeyPressed(HZ_KEY_S)) m_CameraPosition.y -= m_CameraSpeed * dt;
        if (hazel::Input::IsKeyPressed(HZ_KEY_Q)) m_CameraRotation -= m_CameraRotateDegree * dt;
        if (hazel::Input::IsKeyPressed(HZ_KEY_E)) m_CameraRotation += m_CameraRotateDegree * dt;
        // clang-format on
    };

    void OnImGuiRender() override
    {
        ImGui::Begin("Panel");
        ImGui::SliderFloat("m_CameraRotation", &m_CameraRotation, 0, 360);
        ImGui::SliderFloat3("m_CameraPosition", glm::value_ptr(m_CameraPosition), -1, 1);
        ImGui::SliderFloat3("m_SquarePosition", glm::value_ptr(m_SquarePosition), -1, 1);
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
    float     m_CameraSpeed        = 5.f;
    float     m_CameraRotateDegree = 120.f;

    std::shared_ptr<VertexArray> m_VertexArray;
    glm::vec3                    m_SquarePosition = glm::vec3(1.f);


    std::shared_ptr<Shader> m_Shader;
    OrthographicsCamera     m_Camera;
};


} // namespace hazel
class Sandbox : public hazel::App
{
  public:
    Sandbox()
    {
        HZ_INFO("Sandbox construct");
        PushLayer(new hazel::NothingLayer);
        GetWindow().SetVSync(true);
    }
    ~Sandbox() override = default;
};


hazel::App *hazel::CreateApplication()
{
    return new Sandbox;
}
