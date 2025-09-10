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
	innerRadius_ = 1.2f;
	outerRadius_ = 3.0f;
	stemHeight_ = 3.0f;
	time_ = 0.0f;
	addTime_ = 0.02f;
	moonOffset_ = { 75.0f,37.0f };
	moonThickness_ = 2.7f;
	moonRotation_ = 0.0f;
	moonOuterRadius_ = 3.3f;
	for (int i = 0; i < kParticleNum; i++) {
		isActiveParticle_[i] = false;
	}
	spaceEmitFrame_ = 300;
	adOffset_ = 0.0f;
	particleSpeed_ = 0.05f;
	particleRotateSpeed_ = 0.5 * Math::ToRadian;
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
		ImGui::DragFloat("particleSpeed", &particleSpeed_, 0.1f);
		ImGui::DragFloat("particleRotateSpeed", &particleRotateSpeed_, 0.1f);
		ImGui::DragInt("emitFrame", &spaceEmitFrame_);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif
	//エミット
	emitFrame_--;
	if (emitFrame_ <= 0) {
		emitFrame_ = spaceEmitFrame_;
		for (int i = 0; i < kParticleNum; i++) {
			if (!isActiveParticle_[i]) {
				isActiveParticle_[i] = true;
				particlePos_[i] = Vector2{random_.NextFloatRange(3.0f,97.0f),-20.0f};
				particleType_[i] = ParticleType(random_.NextIntRange(0, 3));
				particleRadian_[i] = 0.0f;
				break;
			}

		}
	}

	//更新
	for (int i = 0; i < kParticleNum; i++) {
		if (isActiveParticle_[i]) {
			particlePos_[i].y += particleSpeed_;
			particleRadian_[i] += particleRotateSpeed_;
			if (particlePos_[i].y >= 50.0f) {
				isActiveParticle_[i] = false;
			}
			
		}

	}

}

void BackGround::Draw() {
	int offsetNum = int(Wall::GetInstance()->GetPosition() / space_);
	for (int i = 0; i < 10; i++) {
		TOMATOsEngine::DrawWavingFlower({ offsetNum * space_ +  space_ * i,0.0f }, z_, stemHeight_, time_, 0x666666FF);
	}

	for (int i = 0; i < kParticleNum; i++) {
		if (isActiveParticle_[i]) {
			switch (particleType_[i])
			{
			case kStar:
				TOMATOsEngine::DrawStar2D({particlePos_[i].x + Wall::GetInstance()->GetPosition(),particlePos_[i].y }, outerRadius_, innerRadius_, z_, particleRadian_[i], 0x080808FF);
				break;
			case kSquare:
				TOMATOsEngine::DrawBoxLine3D({ particlePos_[i].x + Wall::GetInstance()->GetPosition(),particlePos_[i].y }, { 3.0f,3.0f },z_, particleRadian_[i], 0x080808FF);
				break;
			case kTriangle:
				TOMATOsEngine::DrawTriangle2D({ particlePos_[i].x + Wall::GetInstance()->GetPosition(),particlePos_[i].y }, 3.0f, z_, particleRadian_[i], 0x080808FF);
				break;
			case kPlus:
				TOMATOsEngine::DrawPlus2D({ particlePos_[i].x + Wall::GetInstance()->GetPosition(),particlePos_[i].y }, 3.0f, 0.5f, z_ , particleRadian_[i], 0x080808FF);
				break;
			default:
				break;
			}
		}
	}
	
	TOMATOsEngine::DrawCrescentMoon({ moonOffset_.x + Wall::GetInstance()->GetPosition() , moonOffset_.y},z_, moonOuterRadius_, moonThickness_,moonRotation_,0x666666FF);
}

void BackGround::ResultDraw() {
	for (int i = 0; i < 20; i++) {
		TOMATOsEngine::DrawWavingFlower({ -4.0f ,0.0f },0.0f, stemHeight_, time_, 0xFFFFFFFF);	
	}
}