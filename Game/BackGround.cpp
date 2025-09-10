#include "BackGround.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"
#include "Player.h"
#include "Wall.h"

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
	z_ = 5.4f;
	y_ = 27.0f;
	innerRadius_ = 2.0f;
	outerRadius_ = 5.0f;
	stemHeight_ = 3.0f;
	time_ = 0.0f;
	addTime_ = 0.02f;
	moonOffset_ = { 75.0f,37.0f };
	moonThickness_ = 2.7f;
	moonRotation_ = 0.0f;
	moonOuterRadius_ = 3.3f;
	for (int i = 0; i < kStarNum_; i++) {
		starPosOffsets_[i] = { random_.NextFloatRange(0.0f,100.0f), random_.NextFloatRange(25.0f,40.0f) };
	}
	starSquare_.radian = 0.0f;
	starSquare_.size = { 0.1f,0.1f };

	adOffset_ = 0.0f;
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
		ImGui::DragFloat2("moonOffset",&moonOffset_.x,0.1f);
		ImGui::DragFloat("moonThickness", &moonThickness_, 0.1f);
		ImGui::DragFloat("moonOuterRadius", &moonOuterRadius_, 0.1f);
		ImGui::DragFloat("moonRotation", &moonRotation_, 0.1f);
		ImGui::DragFloat2("starSquareSize", &starSquare_.size.x, 0.1f);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif
}

void BackGround::Draw() {
	int offsetNum = int(Wall::GetInstance()->GetPosition() / space_);
	for (int i = 0; i < 10; i++) {
		TOMATOsEngine::DrawWavingFlower({ offsetNum * space_ +  space_ * i,0.0f }, z_, stemHeight_, time_, 0x666666FF);
	}
	
	TOMATOsEngine::DrawCrescentMoon({ moonOffset_.x + Wall::GetInstance()->GetPosition() , moonOffset_.y},z_, moonOuterRadius_, moonThickness_,moonRotation_,0x666666FF);
}

void BackGround::ResultDraw() {
	for (int i = 0; i < 20; i++) {
		TOMATOsEngine::DrawWavingFlower({ -4.0f ,0.0f },0.0f, stemHeight_, time_, 0xFFFFFFFF);	
	}
}