
#pragma once

#include "base.h"

#include "layer.h"

namespace hazel {


class HAZEL_API LayerStack
{
    bool bCleand = false;

  protected:
    std::vector<Layer *> m_Layers;
    int                  m_LayerIndex = 0;


  public:
    LayerStack();
    ~LayerStack();

    void Cleanup();

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *overlay);
    void PopLayer(Layer *layer);
    void PopOverlay(Layer *overlay);

    std::vector<Layer *> &GetLayers() { return m_Layers; }
};

}; // namespace hazel