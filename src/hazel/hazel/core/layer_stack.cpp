
#include "layer_stack.h"
#include <algorithm>
namespace hazel {

LayerStack ::LayerStack()
{
}

LayerStack::~LayerStack()
{
    for (Layer *layer : m_Layers)
        delete layer;
}

void LayerStack ::PushLayer(Layer *layer)
{
    m_Layers.emplace(m_Layers.begin() + m_LayerIndex, layer);
    ++m_LayerIndex;
}

void LayerStack ::PushOverlay(Layer *overlay)
{
    m_Layers.emplace_back(overlay);
}

void LayerStack ::PophLayer(Layer *layer)
{
    auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
    if (it != m_Layers.end()) {
        (*it)->OnDetach();
        m_Layers.erase(it);
        --m_LayerIndex;
    }
}

void LayerStack ::PophOverlay(Layer *overlay)
{
    auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
    if (it != m_Layers.end()) {
        (*it)->OnDetach();
        m_Layers.erase(it);
        // --m_LayerInsert;
    }
}

} // namespace hazel