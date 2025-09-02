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
	// フレーム
	fream_TextureHandle_ = TOMATOsEngine::LoadTexture("Resources/frame.png");
	fream_Position_ = { float(TOMATOsEngine::kMonitorWidth) * 0.5f,float(TOMATOsEngine::kMonitorHeight) * 0.5f - 40.0f };
	fream_Size_ = { float(TOMATOsEngine::kMonitorWidth),float(TOMATOsEngine::kMonitorHeight) };
	fream_ColorH_ = 0.319f;
	Initialize();
}

BackGround::~BackGround() {}

void BackGround::Initialize() {
	
}

void BackGround::Update() {
	
}

void BackGround::Draw() {
	// フレーム
	TOMATOsEngine::DrawFrame({ 0.0f,0.0f }, fream_Size_, Vector2::zero, 0.0f, Vector2::zero, {640.0f,480.0f}, fream_TextureHandle_, Color::HSVA(fream_ColorH_, 1.0f, 1.0f, 0.4f));
}