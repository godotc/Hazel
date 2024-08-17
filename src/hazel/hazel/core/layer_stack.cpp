
#include "layer_stack.h"
#include "hazel/core/log.h"
#include <algorithm>
#include <cstddef>
namespace hazel {

LayerStack ::LayerStack()
{
}



LayerStack::~LayerStack()
{
    Cleanup();
}

void LayerStack::Cleanup()
{
    if (!bCleand) {
        for (Layer *layer : m_Layers) {
            if (layer) {
                HZ_CORE_INFO("[layer-cleanup] Deleteing the {}... ", layer->GetName());
                delete layer;
                layer = nullptr;
            }
        }
        bCleand = true;
    }
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

void LayerStack ::PopLayer(Layer *layer)
{
    auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
    if (it != m_Layers.end()) {
        (*it)->OnDetach();
        m_Layers.erase(it);
        --m_LayerIndex;
    }
}

void LayerStack ::PopOverlay(Layer *overlay)
{
    auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
    if (it != m_Layers.end()) {
        (*it)->OnDetach();
        m_Layers.erase(it);
        // --m_LayerInsert;
    }
}

} // namespace hazel