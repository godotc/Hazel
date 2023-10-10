//
// Created by nono on 9/16/23.
//

#include "log.h"

#include "input.h"

namespace hazel {
Input *Input::Get()
{
    return m_Instance;
}
} // namespace hazel