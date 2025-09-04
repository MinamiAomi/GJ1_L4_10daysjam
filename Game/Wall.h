#pragma once

#include "Math/MathUtils.h"
#include "Math/Camera.h"
#include "Math/Random.h"
#include "Border.h"

// 迫ってくる壁
class Wall {
public:
    const float kWallWidth = 2.0f;
    const float kWallHeight = 30.0f;
    const float kCameraOffsetZ = -44.0f;
    const float kCameraRotateY = 33.0f;
    const float kInitializeSpeed = 0.05f;
    const float kBurstDistance = 100.0f;
    const float kBurstEndDistance = 75.0f;
    const float kBurstSpeed = 100.0f;

    static Wall* GetInstance();

    void Initialize();
    void Update();
    void Draw();

    void SetIsMove(bool isMove) { isMove_ = isMove; }
    bool IsMove() const { return isMove_; }
    bool IsBurst() const { return isBurst_; }
    float GetPosition() const { return position_; }
    float GetSpeed() const { return speed_; }
    

private:
    float position_ = 0.0f;
    float speed_ = kInitializeSpeed;
    float burstStartPosition_ = 0.0f;
    float burstElapsedTime_ = 0.0f;
    bool isMove_ = true;
    bool isBurst_ = false;
    Camera* camera_;
    Random::RandomNumberGenerator rng_;
    Border* border_;
};