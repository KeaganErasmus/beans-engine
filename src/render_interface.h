#pragma once
#include "assets.h"
#include "beans_lib.h"

constexpr int MAX_TRANSFORMS = 1000;

struct Transform
{
    iVec2   atlasOffset;
    iVec2   spriteSize;
    Vec2    pos;
    Vec2    size;
};

struct RenderData {
    int transformCount;
    Transform transforms[MAX_TRANSFORMS];
};

static RenderData renderData;


void draw_sprite(SpriteID spriteID, Vec2 pos, Vec2 size) {
    Sprite sprite = get_sprite(spriteID);

    Transform transform     = {};
    transform.pos      = pos;
    transform.size          = size;
    transform.atlasOffset   = sprite.atlasOffset;
    transform.spriteSize    =  sprite.spriteSize;

    renderData.transforms[renderData.transformCount++] = transform;
}

