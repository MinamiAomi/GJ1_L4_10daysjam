#include "Wall.h"

#include <cassert>

#include "TOMATOsEngine.h"
#include "ImGuiManager.h"
#include "Easing.h"

#ifdef _DEBUG
static bool freeCamera = false;
static Vector3 freeCameraPosition;
static Vector3 freeCameraRotate;
#endif // _DEBUG

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
        ImGui::Checkbox("FreeCamera", &freeCamera);
        if (ImGui::BeginMenu("FreeCamera")) {
            ImGui::DragFloat3("Position", &freeCameraPosition.x, 0.1f);
            ImGui::DragFloat3("Rotate", &freeCameraRotate.x, 1.0f);
            camera_->SetPosition(freeCameraPosition);
            camera_->SetRotate(Quaternion::MakeFromEulerAngle(freeCameraRotate * Math::ToRadian));
            if (ImGui::Button("SyncGameCamera")) {
                freeCameraPosition = { position_, kWallHeight * 0.5f, kCameraOffsetZ };
                freeCameraRotate = { 0.0f, kCameraRotateY, 0.0f };
            }
            ImGui::EndMenu();
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
            position_ += speed_;
        }

        float borderPosition = border_->GetBorderSidePos();
        // 距離が離れて動いていないなら
        if (borderPosition - position_ > kBurstDistance && !border_->IsMove()) {
            isBurst_ = true;
            burstElapsedTime_ = 0.0f;
            burstStartPosition_ = position_;
        }
    }
    // バースト
    else {
        burstElapsedTime_ += kDeltaTime;

        float burstEndPosition = border_->GetBorderSidePos() - kBurstEndDistance;
        float distance = burstEndPosition - burstStartPosition_;
        float duration = distance / kBurstSpeed;
        float t = std::clamp(burstElapsedTime_ / duration, 0.0f, 1.0f);
        position_ = burstStartPosition_ + (burstEndPosition - burstStartPosition_) * Easing::OutCubic(t);

        if (t >= 1.0f) {
            isBurst_ = false;
            position_ = burstEndPosition;
        }
    }

    // カメラの位置を固定角
#ifdef _DEBUG
    if (!freeCamera) {
#endif // _DEBUG
        camera_->SetPosition({ position_, kWallHeight * 0.5f, kCameraOffsetZ });
        camera_->SetRotate(Quaternion::MakeForYAxis(kCameraRotateY * Math::ToRadian));
        //camera_->SetRotate(Quaternion::identity);
#ifdef _DEBUG
    }
#endif // _DEBUG
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