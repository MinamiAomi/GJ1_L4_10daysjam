#include "Border.h"

#include "Engine/TOMATOsEngine.h"

#include "Easing.h"

static const float SNAP_THRESHOLD = 0.01f;

void Border::Initialize()
{
	position =
	{ static_cast<float>(TOMATOsEngine::kMonitorWidth),
		static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f };
	size= { 2.0f, static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f };

	velocity = 0.1f;
	easingSpeed = 0.08f;
	pushBackPosition = 0.0f;
	pushBackCoefficient = 20.0f;
}

void Border::Update()
{
#ifdef _DEBUG
	static float addPushBackCount = 0.5f;
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("Border")) {
		ImGui::DragFloat("velocity", &velocity, 0.01f);
		ImGui::DragFloat("easingSpeed", &easingSpeed, 0.01f);
		ImGui::DragFloat("pushBackCoefficient", &pushBackCoefficient, 1.0f);

		ImGui::DragFloat("AddPushBackCount", &addPushBackCount, 1.0f, 0.5f, 5.0f);
		if (ImGui::Button("AddPushBack")) {
			PushBack(addPushBackCount);
		}
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
		else if (position.x >= static_cast<float>(TOMATOsEngine::kMonitorWidth)) {
			position.x = static_cast<float>(TOMATOsEngine::kMonitorWidth);
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
