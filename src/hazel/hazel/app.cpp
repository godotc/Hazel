#include "event/event.h"
#include "event/key_event.h"
#include "hz_pch.h"

#include "app.h"

#include "hazel.h"

#include "glad/glad.h"
#include "imgui/imgui_layer.h"
#include "input.h"
#include "layer.h"
#include "log.h"
#include <GLFW/glfw3.h>

#include <memory>
#include <unistd.h>

namespace hazel {

App *App::Application = nullptr;

App::App()
{
    HZ_INFO("App construct");
    HZ_CORE_ASSERT(!Application, "Already a application instance");
    Application = this;

    m_Window = std::unique_ptr<Window>(Window::Create());
    m_ImGuiLayer = new ImGuiLayer();
    m_LayerStack.PushOverlay(m_ImGuiLayer);

    m_Window->SetEventCallback([this](Event &ev) -> void { this->OnEvent(ev); });

    // VA
    glGenVertexArrays(1, &VA);
    glBindVertexArray(VA);
    // VB
    float vertices[4][3] = {
        {-0.5, -0.5, 0},
        { 0.5, -0.5, 0},
        {-0.5,  0.5, 0},
        { 0.5,  0.5, 0},
    };
    m_VertexBuffer.reset(VertexBuffer::Create((float *)vertices, sizeof(vertices)));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
    // IB
    uint32_t indices[2][3] = {
        {0, 1, 2},
        {2, 1, 3},
    };
    m_IndexBuffer.reset(IndexBuffer::Create((uint32_t *)indices, sizeof(indices) / sizeof(uint32_t)));
    // Shader
    std::string vert = R"(
        #version 330 core
        layout(location =0 ) in vec3 a_Position;
        out vec3 pos;
        void main(){
            gl_Position = vec4(a_Position, 1.f);
            pos = a_Position;
        }
    )";
    std::string frag = R"(
        #version 330 core
        in vec3 pos;
        out vec4 color;
        void main(){
            color = vec4(pos*0.5 + 0.5,0);
        }
    )";
    m_Shader         = std::make_unique<Shader>(vert, frag);
}

void App::Run()
{
    while (bRunning) {
        glClearColor(0.3, 0.5, 0.7, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VA);
        m_Shader->Bind();
        glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

        for (Layer *layer : m_LayerStack.GetLayers())
        {
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
