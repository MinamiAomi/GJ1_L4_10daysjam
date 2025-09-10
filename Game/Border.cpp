#include "Border.h"

#include "Engine/TOMATOsEngine.h"

#include "Easing.h"
#include "Particle/ParticleManager.h"

#include "Wall.h"

#include "Player.h"

//収束値
static const float SNAP_THRESHOLD = 0.01f;

Border* Border::GetInstance()
{
	static Border instance;
	return &instance;
}

void Border::Initialize()
{
	position_ = { Wall::GetInstance()->GetPosition() + Wall::kBurstDistance };
	firstPosition_ = position_;
	easingSpeed_ = 0.08f;
	pushBackPosition_ = position_;
	pushBackCoefficient_ = 4.0f;
	pushBackHipDropCoefficient_ = 1.3f;

	//comboCoefficient_ = 1.05f;
	//comboDuration_ = 2.0f;
	//comboTime_ = 0.0f;

	hitBombNum_ = 0;

	color_ = 0xFFFFFFFF;
}

void Border::Update()
{
#ifdef _DEBUG
	static int addPushBackCount = 1;
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("Border")) {
		ImGui::DragFloat("Position", &position_, 0.1f);
		ImGui::DragFloat("EasingSpeed", &easingSpeed_, 0.01f);
		ImGui::DragFloat("PushBackCoefficient", &pushBackCoefficient_, 1.0f);
		ImGui::DragFloat("PushBackHipDropCoefficient", &pushBackHipDropCoefficient_, 0.1f);
		//ImGui::DragFloat("ComboCoefficient_", &comboCoefficient_, 0.01f);
		//ImGui::DragFloat("ComboDuration_", &comboDuration_, 0.1f);

		ImGui::DragInt("AddPushBackCount", &addPushBackCount, 1, 0);
		if (ImGui::Button("AddPushBack")) {
			PushBack(addPushBackCount);
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

	CalcBomb();

	position_ = Easing::easing(easingSpeed_, position_, pushBackPosition_);


	//近くなったら
	if (std::abs(position_ - pushBackPosition_) < SNAP_THRESHOLD) {
		pushBackPosition_ = position_;
	}
}

void Border::Draw()
{
	float top = kWallHeight;
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
	hitBombNum_ += add;
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


float Border::GetPushBackScore()
{
	if (player_->GetIsHipDrop()) {

		return (hitBombNum_ * pushBackCoefficient_) * pushBackHipDropCoefficient_;
	}
	else {
		return (hitBombNum_ * pushBackCoefficient_);
	}
}

float Border::GetPushBackScore(int num)
{
	if (player_->GetIsHipDrop()) {

		return (num * pushBackCoefficient_) * pushBackHipDropCoefficient_;
	}
	else {
		return (num * pushBackCoefficient_);
	}
}

void Border::CalcBomb()
{
	if (hitBombNum_ != 0) {
		//ヒップドロップし終わりならここ
		if ((!player_->GetIsHipDrop() && prePlayerHipDrop_))
		{
			pushBackPosition_ += (hitBombNum_ * pushBackCoefficient_) * pushBackHipDropCoefficient_;
			hitBombNum_ = 0;
		}
		//ヒップドロップしていなかったら
		else if (!player_->GetIsHipDrop())
		{
			pushBackPosition_ += (hitBombNum_ * pushBackCoefficient_);
			hitBombNum_ = 0;
		}
	}
	prePlayerHipDrop_ = player_->GetIsHipDrop();
}
