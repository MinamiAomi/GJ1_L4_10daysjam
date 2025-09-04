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
	firstPosition_ = position_;
	easingSpeed_ = 0.08f;
	pushBackPosition_ = 0.0f;
	pushBackCoefficient_ = 20.0f;
	pushBackHipDropCoefficient_ = 1.3f;

	color_ = 0xFFFFFFFF;
}

void Border::Update()
{
#ifdef _DEBUG
	static int addPushBackCount = 1;
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("Border")) {
		ImGui::DragFloat("easingSpeed", &easingSpeed_, 0.01f);
		ImGui::DragFloat("pushBackCoefficient", &pushBackCoefficient_, 1.0f);
		ImGui::DragFloat("pushBackHipDropCoefficient", &pushBackHipDropCoefficient_, 0.1f);

		ImGui::DragInt("AddPushBackCount", &addPushBackCount, 1, 0);
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

	float top = wall->kWallHeight;
	float bottom = 0.0f;
	float right = position_ + kWallWidth;
	float left = position_;

	Vector2 bottomLeft = { left, bottom };
	Vector2 topLeft = { left, top };
	Vector2 bottomRight = { right, bottom };
	Vector2 topRight = { right, top };

	TOMATOsEngine::DrawLine3D(bottomLeft, topLeft, color_);
	TOMATOsEngine::DrawLine3D(topLeft, topRight, color_);
	TOMATOsEngine::DrawLine3D(topRight, bottomRight, color_);
	TOMATOsEngine::DrawLine3D(bottomRight, bottomLeft, color_);
}

void Border::PushBack(int add)
{
	pushBackPosition_ = position_ + (add * pushBackCoefficient_);
}

void Border::PushBackHipDrop(int add)
{
	pushBackPosition_ = position_ + ((add * pushBackCoefficient_) * pushBackHipDropCoefficient_);
}

float Border::GetBorderSidePos()
{
	return position_;
}

float Border::GetBorderFirstPos()
{
	return firstPosition_;
}

float Border::GetPushBackPosition()
{
	return pushBackPosition_;
}
