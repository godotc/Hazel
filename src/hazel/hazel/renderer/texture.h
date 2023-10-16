//
// Created by nono on 9/24/23.
//

#ifndef HAZEL_TEXTURE_H
#define HAZEL_TEXTURE_H

#include "hazel/core/core.h"

namespace hazel {

class HAZEL_API Texture
{
  public:
    virtual ~Texture() = default;

    virtual uint32_t GetWidth() const  = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual void Bind(uint32_t slot = 0) const = 0;
    virtual void Unbind() const                = 0;
};


class HAZEL_API Texture2D : public Texture
{
  public:
    [[nodiscard]] static Ref<Texture2D> Create(const std::string &path);
};

} // namespace hazel



#endif // HAZEL_TEXTURE_H
