#pragma once

#include "Math/MathUtils.h"
#include "Math/Camera.h"
#include "Math/Random.h"

// 迫ってくる壁
class Wall {
public:
    const float kWallWidth = 1.0f;
    const float kWallHeight = 20.0f;
    const float kCameraOffsetX = 100;
    const float kCameraOffsetY = 100;
    const float kCameraOffsetZ = 100;

    void Initialize(Camera* camera);
    void Update();
    void Draw();

private:
    float position_;
    float velocity_;
    Camera* camera_;
    Random::RandomNumberGenerator rng_;

};