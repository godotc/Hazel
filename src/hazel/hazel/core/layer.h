
#pragma once

#include "base.h"
#include "window.h"
#include "timestep.h"

#include "hazel/event/event.h"

namespace hazel {

class HAZEL_API Layer
{
  public:
    Layer(const std::string &name = "Layer");
    virtual ~Layer();

  public:
    virtual void OnAttach(){};
    virtual void OnDetach(){};
    virtual void OnUpdate(Timestep timestep){};
    virtual void OnImGuiRender(){};
    virtual void OnEvent(Event &event){};

    inline const std::string &GetName() const { return m_DebugName; }

  protected:
    std::string m_DebugName;
};

}; // namespace hazel