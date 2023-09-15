
#pragma once

#include "hazel/core.h"

#include "layer.h"

namespace hazel {


class HAZEL_API LayerStack
{
  public:
    LayerStack();
    ~LayerStack();

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *overlay);
    void PophLayer(Layer *layer);
    void PophOverlay(Layer *overlay);

    std::vector<Layer *>& GetLayers() { return m_Layers; }

  protected:
    std::vector<Layer *>           m_Layers;
    std::vector<Layer *>::iterator m_LayerInsert;
};

}; // namespace hazel