
#pragma once

#include "event/event.h"
#include "hazel/core.h"
#include "window.h"


namespace hazel {


class HAZEL_API Layer
{
  public:
    Layer(const std::string &name = "Layer");
    virtual ~Layer();

  public:
    virtual void OnAttach(){};
    virtual void OnDetach(){};
    virtual void OnUpdate(){};
    virtual void OnEvent(Event &event){};

    inline const std::string &GetName() const { return m_DebugName; }

  protected:
    std::string m_DebugName;
};

}; // namespace hazel