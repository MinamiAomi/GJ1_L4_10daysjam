#include "Wall.h"

#include <cassert>

#include "TOMATOsEngine.h"
#include "ImGuiManager.h"

Wall* Wall::GetInstance() {
    static Wall instance;
    return &instance;
}

void Wall::Initialize(Camera* camera) {
    assert(camera != nullptr);
    camera_ = camera;

    position_ = 0.0f;
    isMove_ = true;
}

void Wall::Update() {

#ifdef _DEBUG
    ImGui::Begin("InGame");
    if (ImGui::BeginMenu("Wall")) {
        ImGui::DragFloat("position", &position_, 0.1f);
        ImGui::DragFloat("velocity", &velocity_, 0.01f);
        ImGui::Checkbox("isMove", &isMove_);
        ImGui::EndMenu();
    }
    ImGui::End();
#endif // _DEBUG

    // 自動で動く
    if (isMove_) {
        position_ += velocity_;
    }

    camera_->SetPosition({ position_, kWallHeight * 0.5f, kCameraOffsetZ});
    camera_->SetRotate(Quaternion::MakeForYAxis(kCameraRotateY * Math::ToRadian));
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