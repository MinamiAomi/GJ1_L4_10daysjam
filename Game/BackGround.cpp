#include "BackGround.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"
#include "Player.h"

#pragma region 色
float HueToRGB(float p, float q, float t) {
	if (t < 0.0f) t += 1.0f;
	if (t > 1.0f) t -= 1.0f;
	if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
	if (t < 1.0f / 2.0f) return q;
	if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
	return p;
}

void HSLToRGB(float hue, float saturation, float lightness, Vector4& rgb) {
	if (saturation == 0.0f) {
		rgb.x = rgb.y = rgb.z = lightness;
	}
	else {
		float q = (lightness < 0.5f) ? (lightness * (1.0f + saturation)) : (lightness + saturation - lightness * saturation);
		float p = 2.0f * lightness - q;

		rgb.x = HueToRGB(p, q, hue + 1.0f / 3.0f);
		rgb.y = HueToRGB(p, q, hue);
		rgb.z = HueToRGB(p, q, hue - 1.0f / 3.0f);
	}
	rgb.w = 1.0f;
}
#pragma endregion

BackGround::BackGround() {
}

BackGround::~BackGround() {}

void BackGround::Initialize() {
	space_ = 33.0f;
	z_ = 4.4f;
	y_ = 27.0f;
	innerRadius_ = 2.0f;
	outerRadius_ = 5.0f;
	stemHeight_ = 3.0f;
	time_ = 0.0f;
	addTime_ = 0.02f;
}

void BackGround::Update() {
	time_ += addTime_;
#ifdef _DEBUG
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("BackGround")) {
		ImGui::DragFloat("z", &z_, 0.1f);
		ImGui::DragFloat("y", &y_, 0.01f);
		ImGui::DragFloat("space", &space_, 0.01f);
		ImGui::DragFloat("innerRadius", &innerRadius_, 0.01f);
		ImGui::DragFloat("outerRadius", &outerRadius_, 0.01f);
		ImGui::DragFloat("stemHeight", &stemHeight_, 0.01f);
		ImGui::DragFloat("addTime", &addTime_, 0.01f);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif
}

void BackGround::Draw() {
	for (int i = 0; i < 10; i++) {
		TOMATOsEngine::DrawStar2D({0.0f + space_ * i,y_}, innerRadius_,outerRadius_,z_,0x666666FF);
		TOMATOsEngine::DrawWavingFlower({ 0.0f + space_ * i,0.0f }, z_, stemHeight_, time_, 0x666666FF);
	}
}