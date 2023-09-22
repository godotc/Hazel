#include "hz_pch.h"

#include "event/event.h"
#include "event/key_event.h"

#include "app.h"

#include "hazel.h"

#include "glad/glad.h"
#include "imgui/imgui_layer.h"
#include "input.h"
#include "layer.h"
#include "log.h"
#include <GLFW/glfw3.h>

#include "hazel/renderer/render.h"
#include "hazel/renderer/render_command.h"

namespace hazel {


App *App::Application = nullptr;

App::App()
{
    HZ_INFO("App construct");
    HZ_CORE_ASSERT(!Application, "Already a application instance");
    Application = this;

    m_Window     = std::unique_ptr<Window>(Window::Create());
    m_ImGuiLayer = new ImGuiLayer();
    m_LayerStack.PushOverlay(m_ImGuiLayer);

    m_Window->SetEventCallback([this](Event &ev) -> void { this->OnEvent(ev); });

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
        layout(location =0 ) in vec3 a_Position;
        layout(location =1 ) in vec4 a_Color;

        out vec3 pos;
        out vec4 color;

        void main(){
            gl_Position = vec4(a_Position, 1.f);
            pos = a_Position;
            color = a_Color;
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

void App::Run()
{
    while (bRunning) {

        RenderCommand::SetClearColor({0.3, 0.5, 0.7, 1});
        RenderCommand::Clear();

        Render::BeginScene();

        m_Shader->Bind();
        Render::Submit(m_VertexArray);

        Render::EndScene();
        //        Renderer::Flush(0);


        for (Layer *layer : m_LayerStack.GetLayers()) {
            layer->OnUpdate();
        }

        m_ImGuiLayer->Begin();
        for (Layer *layer : m_LayerStack.GetLayers()) {
            layer->OnImGuiRender();
        }
        m_ImGuiLayer->End();

        m_Window->OnUpdate();
    }

    for (auto layer : m_LayerStack.GetLayers()) {
        layer->OnDetach();
    }
}
void App::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
}

void App::PopLayer(Layer *layer)
{
    m_LayerStack.PophLayer(layer);
}

void App::OnEvent(Event &ev)
{
    if (!ev.IsInCategory(EventCategoryInput)) {
        HZ_CORE_INFO(ev.to_string());
    }

    EventDispatcher dispatcher(ev);
    dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent &ev) -> bool { return OnWindowClose(ev); });
    dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent &ev) -> bool { return OnKeyPressed(ev); });

    auto &Layers = m_LayerStack.GetLayers();
    for (auto it = Layers.end(); it != Layers.begin();) {
        (*--it)->OnEvent(ev);
        if (ev.bHandled)
            break;
    }
}

bool App::OnKeyPressed(KeyPressedEvent &ev)
{
    return false;
}

bool App::OnWindowClose(WindowCloseEvent &ev)
{
    bRunning = false;
    return true;
}
} // namespace hazel
