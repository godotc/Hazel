//
// Created by nono on 10/11/23.
//

#include "hazel.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "platform/opengl/opengl_shader.h"
#include "utils/path.h"
namespace hazel {
class ExampleLayer : public hazel::Layer
{
  public:
    ExampleLayer()
        : Layer("nothing"),
          m_CameraController(-1.6 / 0.9, true)
    {

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

        // VA
        m_VertexArray = VertexArray::Create();
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer>  m_IndexBuffer;
        {
            // VB
            m_VertexBuffer = VertexBuffer::Create((float *)vertices, sizeof(vertices));
            // Layout
            m_VertexBuffer->SetLayout({
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float2, "a_TexCoord"},
            });
            // IB
            m_IndexBuffer = IndexBuffer::Create((uint32_t *)indices, sizeof(indices) / sizeof(uint32_t));
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

            uniform sampler2D u_Texture;

            out vec4 out_color;

            void main(){
                out_color =  texture(u_Texture, v_TexCoord);
            }
        )";
        //        m_Shader         = hazel::Shader::Create("texture", vert, frag);

        m_ShaderLibrary->Load("Texture", FPath("res/shader/texture.glsl"));

        m_Texture     = hazel::Texture2D::Create(FPath("res/texture/face.png"));
        m_ArchTexture = hazel::Texture2D::Create(FPath("res/texture/arch.png"));
    }

    void OnUpdate(Timestep ts) override
    {
        // HZ_TRACE("Delta time : {}s {}ms ", ts.GetSeconds(), ts.GetMiliseconds());

        m_CameraController.OnUpdate(ts);

        RenderCommand::SetClearColor({0.3, 0.5, 0.7, 1});
        RenderCommand::Clear();

        Render::BeginScene(m_CameraController.GetCamera());
        {
            //            m_CameraController.GetCamera().SetRotation(m_CameraRotation);
            //            m_CameraController.GetCamera().SetPosition(m_CameraPosition);

            glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));

            auto TexShader = m_ShaderLibrary->Get("Texture");

            // m_Texture->Bind();
            // dynamic_pointer_cast<OpenGLShader>(TexShader)->UploadUniformInt("u_Texture", 0);
            // Render::Submit(TexShader, m_VertexArray, glm::scale(glm::mat4(1), glm::vec3(1.5)));

            // m_ArchTexture->Bind(0);
            // dynamic_pointer_cast<OpenGLShader>(TexShader)->UploadUniformInt("u_Texture", 0);
            // Render::Submit(TexShader, m_VertexArray,
            //                glm::translate(
            //                    glm::scale(glm::mat4(1), glm::vec3(1.5)),
            //                    {-0.5, -0.2, 0}));
        }
        Render::EndScene();
    };

    void OnImGuiRender() override
    {
        ImGui::Begin("Panel");
        //        ImGui::SliderFloat("m_CameraRotation", &m_CameraRotation, 0, 360);
        //        ImGui::SliderFloat3("m_CameraPosition", glm::value_ptr(m_CameraPosition), -1, 1);
        ImGui::SliderFloat3("m_SquarePosition", glm::value_ptr(m_SquarePosition), -1, 1);
        ImGui::End();
    }

    void OnEvent(hazel::Event &event) override
    {
        m_CameraController.OnEvent(event);

        hazel::EventDispatcher dispatcher(event);
    }

  private:

    Ref<VertexArray> m_VertexArray;
    glm::vec3        m_SquarePosition = glm::vec3(1.f);

    Ref<ShaderLibrary> m_ShaderLibrary{new ShaderLibrary};

    Ref<Texture2D>                m_Texture, m_ArchTexture;
    OrthographicsCameraController m_CameraController;
};


} // namespace hazel