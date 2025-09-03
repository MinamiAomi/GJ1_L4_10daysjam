#pragma once

#include "Math/MathUtils.h"
#include "Math/Camera.h"
#include "Math/Random.h"

// 迫ってくる壁
class Wall {
public:
    const float kWallWidth = 2.0f;
    const float kWallHeight = 30.0f;
    const float kCameraOffsetZ = -44.0f;
    const float kCameraRotateY = 33.0f;

    static Wall* GetInstance();

    void Initialize(Camera* camera);
    void Update();
    void Draw();

    void SetIsMove(bool isMove) { isMove_ = isMove; }
    float GetPosition() { return position_; }

private:
    float position_ = 0.0f;
    float velocity_ = 0.0f;
    bool isMove_ = true;
    Camera* camera_;
    Random::RandomNumberGenerator rng_;

};