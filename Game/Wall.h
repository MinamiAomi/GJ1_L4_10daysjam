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
    static constexpr float kCameraOffsetZ = -44.0f;
    static constexpr float kCameraRotateY = 33.0f;
    static constexpr float kInitializeSpeed = 0.05f;
    static constexpr float kBurstDistance = 100.0f;
    static constexpr float kBurstEndDistance = 75.0f;
    static constexpr float kBurstSpeed = 100.0f;
    static constexpr int kNumBurstEffects = 5;

    class BurstEffect {
    public:
        void Initialize(Random::RandomNumberGenerator* rng, float borderPosition);
        void Draw(float wallPosition);

    private:
        float offset_;
        Vector2 position_;

    };

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
    std::vector<BurstEffect> burstEffects_;

};