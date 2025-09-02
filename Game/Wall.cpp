#include "Wall.h"

#include <cassert>

#include "TOMATOsEngine.h"

void Wall::Initialize(Camera* camera) {
    assert(camera != nullptr);
    camera_ = camera;

    position_ = 0.0f;
}

void Wall::Update() {
    // 自動で動く
    position_ += velocity_;
}

void Wall::Draw() {

    TOMATOsEngine::DrawLine3D({ position_, 0.0f }, { position_, kWallHeight }, 0xFFFFFFFF);
    TOMATOsEngine::DrawLine3D({ position_ - kWallWidth, 0.0f }, { position_ - kWallWidth, kWallHeight }, 0xFFFFFFFF);

    const uint32_t numNoiseLines = 20;
    for (uint32_t i = 0; i < numNoiseLines; ++i) {
        Vector2 start = { position_, rng_.NextFloatRange(0.0f, kWallHeight) };
        Vector2 end = {position_ - kWallWidth, rng_.NextFloatRange(0.0f, kWallHeight) };
        TOMATOsEngine::DrawLine3D(start, end, 0xFFFFFFFF);
    }
}