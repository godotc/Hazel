
#pragma once


#include "glm/ext/vector_float2.hpp"
#include "hazel/core/base.h"
#include "texture.h"

namespace hazel {

class SubTexture2D
{
    Ref<Texture2D> m_Textrue;
    glm::vec2      m_TexCoords[4];

  public:
    SubTexture2D(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max);

    static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D> &texture, const glm::vec2 &coords, const glm::vec2 &block_size, const glm::vec2 &sprite_size);
    static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D> &texture, const glm::vec2 &coords, const glm::vec2 &block_size, const glm::vec2 &sprite_size, const float margin);

    const Ref<Texture2D> GetTexture() const { return m_Textrue; }
    const glm::vec2     *GetTexccord() const { return m_TexCoords; }
};

} // namespace hazel