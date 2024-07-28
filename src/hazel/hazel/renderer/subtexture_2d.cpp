#include "subtexture_2d.h"
#include "glm/ext/vector_float2.hpp"
#include "hazel/core/base.h"
#include "hazel/renderer/texture.h"
#include <algorithm>
#include <cstdio>

namespace hazel {

// static int ____i = 0;
SubTexture2D::SubTexture2D(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max)
{
    // I make the sequence wrong !
    Tm_Texture     = texture;
    m_TexCoords[0] = {min.x, min.y};
    m_TexCoords[1] = {max.x, min.y};
    m_TexCoords[2] = {max.x, max.y};
    m_TexCoords[3] = {min.x, max.y};

    // ++____i;
    // HZ_CORE_INFO("Subtexture create, the ref of master texture path: {}, and {}", m_Texture->GetTextureID(), ____i);
}

SubTexture2D::~SubTexture2D()
{

    // --____i;
    // HZ_CORE_INFO("Subtexture destruction, the ref of master texture path: {}, and {}", m_Texture->GetTextureID(), ____i);
}

/**
 * @brief
 *
 *  @notice: due the vertical flip loading texture
 *  like a 12x11  sheet
 *  left-bot is (0,0)   right-top is (11,10):
 *      y
 *      |
 *      |
 *      |
 *      *--------->x
 *    (0,0)
 *
 * @param texture parent_texture
 * @param coords the (n,m) of sub cell
 * @param blocksize each block size, like (16*16) (256*256) the determine the bot-left  of the nth x and mth y block
 * @param sprite_size actual the top right: bot-left (16,16) then add (sprite_size.x, sprite_size.y)
 * @return Ref<SubTexture2D>
 */
Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D> &texture, const glm::vec2 &coords, const glm::vec2 &block_size, const glm::vec2 &sprite_size)
{
    float min_x = coords.x * block_size.x / texture->GetWidth();  // x from left to right
    float min_y = coords.y * block_size.x / texture->GetHeight(); // y from bottom to top
    float max_x = min_x + sprite_size.x / texture->GetWidth();
    float max_y = min_y + sprite_size.y / texture->GetHeight();

    glm::vec2 min = {min_x, min_y};
    glm::vec2 max = {max_x, max_y};

    return CreateRef<SubTexture2D>(texture, min, max);
}


/**
 * @brief
 * @param margin some sheet will left a margin with some px as edge of each block,
 *  BUT I think use packed better!
 *  Cause' it's too hard to draw a item with 2 or more block, but within its margin -.-
 */
Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D> &texture, const glm::vec2 &coords, const glm::vec2 &block_size, const glm::vec2 &sprite_size, const float margin)
{
    if (margin <= 0) {
        return CreateFromCoords(texture, coords, block_size, sprite_size);
    }

    float left_bot_x = coords.x * (block_size.x + margin); // x from left to right
    float left_bot_y = coords.y * (block_size.y + margin); // y from bottom to top

    float right_top_x = left_bot_x + (sprite_size.x + margin);
    float right_top_y = left_bot_y + (sprite_size.y + margin);

    //  to remove the margin on the rightest col or most top row
    if (coords.x == (int)(texture->GetWidth() / block_size.x - 1)) {
        right_top_x -= margin;
    }
    // printf("%f %f %f\n", (float)texture->GetWidth(), block_size.x, texture->GetWidth() / block_size.x);
    if (coords.y == (int)(texture->GetHeight() / block_size.y - 1)) {
        right_top_y -= margin;
    }

    glm::vec2 min = {left_bot_x / texture->GetWidth(), left_bot_y / texture->GetHeight()};
    glm::vec2 max = {right_top_x / texture->GetWidth(), right_top_y / texture->GetHeight()};


    return CreateRef<SubTexture2D>(texture, min, max);
}

} // namespace hazel