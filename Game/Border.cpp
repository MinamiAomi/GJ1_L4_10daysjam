#include "Border.h"

#include "Engine/TOMATOsEngine.h"

#include "Easing.h"

//収束値
static const float SNAP_THRESHOLD = 0.01f;

Border* Border::GetInstance()
{
	static Border instance;
	return &instance;
}

void Border::Initialize()
{
	position_ =
	{ static_cast<float>(TOMATOsEngine::kMonitorWidth),
		static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f };
	size_ = { 2.0f, static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f };

	velocity_ = 0.1f;
	easingSpeed_ = 0.08f;
	pushBackPosition_ = 0.0f;
	pushBackCoefficient_ = 20.0f;
}

void Border::Update()
{
#ifdef _DEBUG
	static float addPushBackCount = 0.5f;
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("Border")) {
		ImGui::DragFloat("velocity", &velocity_, 0.01f);
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
	if (pushBackPosition_ == 0.0f) {
		position_.x -= velocity_;
	}
	else {
		position_.x = Easing::easing(easingSpeed_, position_.x, pushBackPosition_);
		//近くなったら
		if (std::abs(position_.x - pushBackPosition_) < SNAP_THRESHOLD) {
			position_.x = pushBackPosition_;
			pushBackPosition_ = 0.0f;
		}
		else if (position_.x >= static_cast<float>(TOMATOsEngine::kMonitorWidth)) {
			position_.x = static_cast<float>(TOMATOsEngine::kMonitorWidth);
			pushBackPosition_ = 0.0f;
		}
	}

	position_.x = std::clamp(position_.x, 0.0f, static_cast<float>(TOMATOsEngine::kMonitorWidth));
	//ゲームオーバー
	if (position_.x <= 0) {

	}
}

void Border::Draw()
{
	TOMATOsEngine::DrawRect(
		{ position_.x - size_.x, position_.y - size_.y },
		{ position_.x + size_.x, position_.y + size_.y },
		0xFFFFFFFF
	);
}

void Border::PushBack(float add)
{
	pushBackPosition_ = position_.x + (add * pushBackCoefficient_);
}
