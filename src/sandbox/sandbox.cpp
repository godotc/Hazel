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
        float vertices[4][5] = {
            {-0.5, -0.5, 0,  0, 0},
            { 0.5, -0.5, 0,  1, 0},
            {-0.5,  0.5, 0, -1, 1},
            { 0.5,  0.5, 0,  1, 1},
        };

        uint32_t indices[2][3] = {
            {0, 1, 2},
            {2, 1, 3},
        };

        // VA
        m_VertexArray.reset(VertexArray::Create());
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer>  m_IndexBuffer;
        {
            // VB
            m_VertexBuffer.reset(VertexBuffer::Create((float *)vertices, sizeof(vertices)));
            // Layout
            m_VertexBuffer->SetLayout({
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float2, "a_TexCoord"},
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
            layout (location =1 ) in vec2 a_TexCoord;

            out vec3 pos;
            out vec2 v_TexCoord;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;

            void main(){
                pos = a_Position;
                v_TexCoord = a_TexCoord;
                gl_Position = u_ViewProjection *  u_Transform * vec4(a_Position, 1.f);
            }
        )";
        std::string frag = R"(
            #version 330 core

            in vec3 pos;
            in vec2 v_TexCoord;

            uniform vec4  u_Color  = vec4(1.f);

            out vec4 out_color;

            void main(){
                out_color =  u_Color * vec4(v_TexCoord, 1, 1);
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

        Render::Submit(m_Shader, m_VertexArray, glm::scale(glm::mat4(1), glm::vec3(1.5)));
        Render::EndScene();



        // clang-format off
        {
            if (hazel::Input::IsKeyPressed(HZ_KEY_A)) m_CameraPosition.x -= m_CameraSpeed * dt;
            if (hazel::Input::IsKeyPressed(HZ_KEY_D)) m_CameraPosition.x += m_CameraSpeed * dt;
            if (hazel::Input::IsKeyPressed(HZ_KEY_W)) m_CameraPosition.y += m_CameraSpeed * dt;
            if (hazel::Input::IsKeyPressed(HZ_KEY_S)) m_CameraPosition.y -= m_CameraSpeed * dt;
            if (hazel::Input::IsKeyPressed(HZ_KEY_Q)) m_CameraRotation -= m_CameraRotateDegree * dt;
            if (hazel::Input::IsKeyPressed(HZ_KEY_E)) m_CameraRotation += m_CameraRotateDegree * dt;
        }
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

    Ref<VertexArray> m_VertexArray;
    glm::vec3        m_SquarePosition = glm::vec3(1.f);


    Ref<Shader>         m_Shader;
    OrthographicsCamera m_Camera;
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
