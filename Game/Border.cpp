#include "Border.h"

#include "Engine/TOMATOsEngine.h"

#include "Easing.h"

#include "Wall.h"

//収束値
static const float SNAP_THRESHOLD = 0.01f;

Border* Border::GetInstance()
{
	static Border instance;
	return &instance;
}

void Border::Initialize()
{
	position_ = { Wall::GetInstance()->GetPosition() + TOMATOsEngine::kMonitorWidth };

	easingSpeed_ = 0.08f;
	pushBackPosition_ = 0.0f;
	pushBackCoefficient_ = 20.0f;

	color_ = 0xFFFFFFFF;
}

void Border::Update()
{
#ifdef _DEBUG
	static float addPushBackCount = 0.5f;
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("Border")) {
		ImGui::DragFloat("easingSpeed", &easingSpeed_, 0.01f);
		ImGui::DragFloat("pushBackCoefficient", &pushBackCoefficient_, 1.0f);

		ImGui::DragFloat("AddPushBackCount", &addPushBackCount, 1.0f, 0.5f, 5.0f);
		if (ImGui::Button("AddPushBack")) {
			PushBack(addPushBackCount);
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

	//押し戻しがなければ
	if (pushBackPosition_ != 0.0f) {
		position_ = Easing::easing(easingSpeed_, position_, pushBackPosition_);
		//近くなったら
		if (std::abs(position_ - pushBackPosition_) < SNAP_THRESHOLD) {
			position_ = pushBackPosition_;
			pushBackPosition_ = 0.0f;
		}
	}
}

void Border::Draw()
{
	const auto& wall = Wall::GetInstance();

	float t = wall->kWallHeight;
	float b = 0.0f;
	float r = position_ + kWallWidth;
	float l = position_;
	TOMATOsEngine::DrawLine3D({ l, b }, { l, t }, color_);
	TOMATOsEngine::DrawLine3D({ l, b }, { r, b }, color_);

	TOMATOsEngine::DrawLine3D({ r,  t }, { l ,t}, color_);
	TOMATOsEngine::DrawLine3D({ r,  t }, { l, b}, color_);

}

void Border::PushBack(float add)
{
	pushBackPosition_ = position_ + (add * pushBackCoefficient_);
}

float Border::GetBorderSidePos()
{
	return position_;
}

float Border::GetBorderCenterPos()
{
	return position_ + (kWallWidth * 0.5f);
}
