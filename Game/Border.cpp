#include "Border.h"

#include "Engine/TOMATOsEngine.h"

#include "Easing.h"

static const float SNAP_THRESHOLD = 0.01f;

void Border::Initialize()
{
	position =
	{ static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,
		static_cast<float>(TOMATOsEngine::kMonitorHeight) };

	velocity = 0.1f;
	easingSpeed = 0.05f;
	pushBackCoefficient = 10.0f;
}

void Border::Update()
{
#ifdef _DEBUG
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("Border")) {
		ImGui::DragFloat("velocity", &velocity, 0.01f);
		ImGui::DragFloat("easingSpeed", &easingSpeed, 0.01f);
		ImGui::DragFloat("pushBackCoefficient", &pushBackCoefficient, 1.0f);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

	//押し戻しがなければ
	if (pushBackPosition == 0.0f) {
		position.x -= velocity;
	}
	else {
		position.x = Easing::easing(easingSpeed, position.x, pushBackPosition);
		//近くなったら
		if (std::abs(position.x - pushBackPosition) < SNAP_THRESHOLD) {
			position.x = pushBackPosition;
			pushBackPosition = 0.0f;
		}
	}

	position.x = std::clamp(position.x, 0.0f, static_cast<float>(TOMATOsEngine::kMonitorWidth));
	//ゲームオーバー
	if (position.x <= 0) {

	}
}

void Border::Draw()
{
	static const Vector2 size = { 1.0f, static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f };
	TOMATOsEngine::DrawRect(
		{ position.x - size.x, position.y - size.y },
		{ position.x + size.x, position.y + size.y },
		0xFFFFFFFF
		);
}

void Border::PushBack(float add)
{
	pushBackPosition = position.x + (add * pushBackCoefficient);
}
