#include "Wall.h"

#include <cassert>

#include "TOMATOsEngine.h"
#include "ImGuiManager.h"
#include "Easing.h"

Wall* Wall::GetInstance() {
    static Wall instance;
    return &instance;
}

void Wall::Initialize() {
    camera_ = Camera::GetInstance();
    border_ = Border::GetInstance();

    position_ = 0.0f;
    isMove_ = true;
    isBurst_ = false;
}

void Wall::Update() {

    assert(camera_ != nullptr);
    assert(border_ != nullptr);

#ifdef _DEBUG
    ImGui::Begin("InGame");
    if (ImGui::BeginMenu("Wall")) {
        ImGui::DragFloat("position", &position_, 0.1f);
        ImGui::DragFloat("speed", &speed_, 0.01f);
        ImGui::Checkbox("isMove", &isMove_);
        if (ImGui::Button("burst")) {
            border_->PushBack(10);
        }
        ImGui::EndMenu();
    }
    ImGui::End();
#endif // _DEBUG

    // バースト条件
    const float kDeltaTime = 1.0f / 60.0f;

    // 通常動作
    if (!isBurst_) {
        // 自動で動く
        if (isMove_) {
            speed_ = kInitializeSpeed + position_ * kSpeedFactor;
            speed_ = std::min(speed_, kSpeedLimit);
            position_ += speed_;
        }

        float borderPosition = border_->GetBorderSidePos();
        // 距離が離れて動いていないなら
        if (borderPosition - position_ > kBurstDistance && !border_->IsMove()) {
            isBurst_ = true;
            burstElapsedTime_ = 0.0f;
            burstStartPosition_ = position_;
        }

        // 通常時のカメラ
        camera_->SetPosition({ position_ + kCameraOffsetX, kCameraOffsetY, kCameraOffsetZ });
        camera_->SetRotate(Quaternion::identity);
    }
    // バースト
    else {
        burstElapsedTime_ += kDeltaTime;

        // 壁移動
        float burstEndPosition = border_->GetBorderSidePos() - kBurstEndDistance;
        float distance = burstEndPosition - burstStartPosition_;
        float duration = distance / kBurstSpeed;
        float burstT = std::clamp(burstElapsedTime_ / duration, 0.0f, 1.0f);
        position_ = burstStartPosition_ + (burstEndPosition - burstStartPosition_) * Easing::OutCubic(burstT);

        if (burstT >= 1.0f) {
            isBurst_ = false;
            position_ = burstEndPosition;
        }

        // 
        const float transitionLength = kBurstCameraTransition / duration;
        float transitionT = 0.0f;
        // 始まり演出
        if (burstT <= transitionLength) {
            transitionT = burstT / transitionLength;
        }
        else if (burstT < 1.0f - transitionLength) {
            transitionT = 1.0f;
        }
        // 終わり演出
        else {
            transitionT = 1.0f - (burstT - (1.0f - transitionLength)) / transitionLength;
        }
                 
        auto cameraPosition = Vector3::Lerp(transitionT, { position_ + kCameraOffsetX, kCameraOffsetY, kCameraOffsetZ }, { position_, kCameraOffsetY, kCameraOffsetZ });
        auto cameraRotate = Quaternion::Slerp(transitionT, Quaternion::identity, Quaternion::MakeForYAxis(kBurstCameraRotate * Math::ToRadian));

        // 通常時のカメラ
        camera_->SetPosition(cameraPosition);
        camera_->SetRotate(cameraRotate);
    }

    camera_->UpdateMatrices();
    TOMATOsEngine::SetCameraMatrix(camera_->GetViewProjectionMatrix());
}

void Wall::Draw() {

    TOMATOsEngine::DrawLine3D({ position_, 0.0f }, { position_, kWallHeight }, 0xFFFFFFFF);
    TOMATOsEngine::DrawLine3D({ position_ - kWallWidth, 0.0f }, { position_ - kWallWidth, kWallHeight }, 0xFFFFFFFF);

    for (uint32_t i = 0; i < uint32_t(kWallHeight); ++i) {
        Vector2 start = { position_, i + 1.0f };
        Vector2 end = { position_ - kWallWidth, i + 1.0f };
        TOMATOsEngine::DrawLine3D(start, end, 0xFFFFFFFF);
    }
}