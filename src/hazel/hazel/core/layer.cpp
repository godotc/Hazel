#include "layer.h"
#include "hazel/core/log.h"

namespace hazel {

Layer::Layer(const std::string &name) : m_DebugName(name) {}

Layer::~Layer()
{
    HZ_CORE_TRACE("[Layer Destruction] {} completed  destruction....", GetName());
}
} // namespace hazel
