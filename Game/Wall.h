#pragma once

#include <list>

#include "Math/MathUtils.h"
#include "Math/Camera.h"
#include "Math/Random.h"
#include "Border.h"

// 迫ってくる壁
class Wall {
public:
    static constexpr float kWallWidth = 2.0f;
    static constexpr float kWallHeight = 30.0f;
    
    static constexpr float kCameraOffsetX = 30.0f;
    static constexpr float kCameraOffsetY = kWallHeight * 0.5f;
    static constexpr float kCameraOffsetZ = -44.0f;

    static constexpr float kBurstCameraRotate = 33.0f;
    static constexpr float kBurstCameraTransition = 0.5f;

    static constexpr float kInitializeSpeed = 0.05f;
    static constexpr float kSpeedFactor = 0.0001f;
    static constexpr float kSpeedLimit = 0.5f;

    static constexpr float kBurstDistance = 100.0f;
    static constexpr float kBurstEndDistance = 60.0f;
    static constexpr float kBurstSpeed = 100.0f;

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