#pragma once
#include "beans_lib.h"

enum SpriteID {
    SPRITE_BEAN,

    SPRITE_COUNT
};

struct Sprite
{
    iVec2 atlasOffset;
    iVec2 spriteSize;
};

Sprite get_sprite(SpriteID spriteID) {

    Sprite sprite = {};

    switch (spriteID)
    {
    case SPRITE_BEAN: {
        sprite.atlasOffset = {0, 0};
        sprite.spriteSize = {16, 16};
    }    
    default:
        break;
    }

    return sprite;
}