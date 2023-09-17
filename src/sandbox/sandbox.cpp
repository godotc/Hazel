#include <hazel.h>
#include <hazel/imgui/imgui_layer.h>
#include <ostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/matrix.hpp"

glm::mat4 camera(float translate, const glm::vec2 &roatation)
{
    glm::mat4 proj  = glm::perspective(glm::radians(45.f), 4.f / 3.f, 0.1f, 100.f);
    glm::mat4 view  = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -translate));
    view            = glm::rotate(view, roatation.y, glm::vec3(-1.f, 0.f, 0.f));
    view            = glm::rotate(view, roatation.x, glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(0.5f));
    return proj * view * model;
};

class NothingLayer : public hazel::Layer
{
  public:
    NothingLayer() : Layer("nothing")
    {
        auto cam = camera(5.f, {1.f, 1.f});
        auto a   = cam[0][1];
    }

    void OnUpdate() override
    {
    }


    void OnImGuiRender() override
    {
        static bool bShow = true;
        ImGui::ShowDemoWindow(&bShow);

        ImGui::Begin("hello world");
        ImGui::Text("heeeeeee");
        ImGui::End();
    }

    void OnEvent(hazel::Event &event) override
    {
        if (event.GetEventType() == hazel::EventType::KeyPressed) {
            auto kc = static_cast<hazel::KeyPressedEvent &>(event).GetKeyCode();
            HZ_TRACE("{}, is F1 button? {}", (char)kc, kc == HZ_KEY_F1);
        }
    }
};


class Sandbox : public hazel::App
{
  public:
    Sandbox()
    {
        HZ_INFO("Sandbox construct");
        PushLayer(new NothingLayer);
    }

    ~Sandbox() override
    {
    }
};



hazel::App *hazel::CreateApplication()
{
    return new Sandbox;
}
