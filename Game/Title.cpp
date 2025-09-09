#include "Title.h"

#include <array>

#include "TOMATOsEngine.h" 
#include "Math/Color.h"
#include "Easing.h"

#include "Math/Random.h"

void Title::Initialize()
{
	titleText_.Initialize();
}

void Title::Update()
{

	animationTime_ += 1.0f / 60.0f;
	titleText_.AnimateTitleLoop();
}

void Title::Draw()
{
	titleText_.Draw();
}

void Title::TitleText::Initialize()
{
	position_ = { 0.0f, 10.0f };
	scale_ = 10.0f;
	space_ = 10.0f;
	outlineThickness_ = 0.1f;
	color_ = { 1.0f,1.0f,1.0f,1.0f };
	rotateAngle_ = 0.0f;
	rotateSpeed_ = -0.5f;
	rotateRadius_ = 20.0f;
	jitterAmount_ = 0.0f;
}


void Title::TitleText::AnimateFirst(float t)
{
#ifdef _DEBUG
	ImGui::Begin("Title Settings");
	ImGui::DragFloat2("Position", &position_.x, 1.0f);
	ImGui::DragFloat("Scale", &scale_, 1.0f, 10.0f, 500.0f);
	ImGui::DragFloat("Space", &space_, 1.0f, 10.0f, 500.0f);
	ImGui::DragFloat("Thickness", &outlineThickness_, 0.001f, 0.0f, 0.5f);
	ImGui::DragFloat("RotateSpeed", &rotateSpeed_, 0.1f);
	ImGui::DragFloat("RotateRadius", &rotateRadius_, 0.1f);
	ImGui::DragFloat("JitterAmount", &jitterAmount_, 0.1f, 0.0f);
	ImGui::End();
#endif // _DEBUG
	scale_ = Easing::easing(Easing::InOutBack(t), kScale * 500.0f, kScale);
	rotateAngle_ = 0.0f;
}

void Title::TitleText::AnimateTitleLoop()
{
#ifdef _DEBUG
	ImGui::Begin("Title Settings");
	ImGui::DragFloat2("Position", &position_.x, 1.0f);
	ImGui::DragFloat("Scale", &scale_, 1.0f, 10.0f, 500.0f);
	ImGui::DragFloat("Space", &space_, 1.0f, 10.0f, 500.0f);
	ImGui::DragFloat("Thickness", &outlineThickness_, 0.001f, 0.0f, 0.5f);
	ImGui::DragFloat("RotateSpeed", &rotateSpeed_, 0.1f);
	ImGui::DragFloat("RotateRadius", &rotateRadius_, 0.1f);
	ImGui::DragFloat("JitterAmount", &jitterAmount_, 0.1f);
	ImGui::End();
#endif // _DEBUG

	// 公転の角度を更新
	rotateAngle_ += rotateSpeed_ * (1.0f / 60.0f);
	rotateAngle_ = std::fmodf(rotateAngle_, 2.0f * Math::Pi);
}

void Title::TitleText::Draw()
{
	Random::RandomNumberGenerator rnd{};
	uint32_t colorInt = Color(color_);

	int numCharacters = static_cast<int>(titleText_.size());
	if (numCharacters == 0) return;

	if (std::abs(rotateRadius_) < 0.1f) return;

	//文字間隔
	const float angleStep = space_ / rotateRadius_;

	//全体の幅
	const float totalArcAngle = (numCharacters - 1) * angleStep;

	//最初の角度
	const float startAngle = rotateAngle_ - (totalArcAngle * 0.5f);

	for (int charIndex = 0; charIndex < numCharacters; ++charIndex) {
		const auto& characterData = titleText_[charIndex];

		//公転角度を算出
		float charOrbitAngle = startAngle + (static_cast<float>(charIndex) * angleStep);


		float displayAngle = charOrbitAngle - (Math::Pi * 0.5f);
		// 3D空間での文字の中心位置を計算
		Vector3 charCenterPosition;
		charCenterPosition.x = position_.x + rotateRadius_ * std::cosf(displayAngle);
		charCenterPosition.y = position_.y;
		charCenterPosition.z = rotateRadius_ * std::sinf(displayAngle);

		for (size_t i = 0; i < characterData.size(); i += 2) {
			Vector2 start2D = characterData[i] * scale_;
			Vector2 end2D = characterData[i + 1] * scale_;
			Vector2 dir2D = end2D - start2D;
			if (dir2D.LengthSquare() == 0.0f) continue;
			dir2D.Normalize();
			Vector2 perp2D = { -dir2D.y, dir2D.x };

			float thickness = outlineThickness_ * scale_;
			Vector2 halfPerp = perp2D * (thickness / 2.0f);

			std::array<Vector2, 4> corners2D = {
				start2D + halfPerp, end2D + halfPerp,
				end2D - halfPerp, start2D - halfPerp
			};

			std::array<Vector3, 4> corners3D;
			for (int j = 0; j < 4; ++j) {
				corners3D[j] = {
					corners2D[j].x + charCenterPosition.x,
					corners2D[j].y + charCenterPosition.y,
					charCenterPosition.z
				};
			}

			//ジェッター用
			std::array<Vector3, 4> jitteredCorners;
			for (int j = 0; j < 4; ++j) {
				Vector3 jitter = {
					rnd.NextFloatRange(-jitterAmount_, jitterAmount_),
					rnd.NextFloatRange(-jitterAmount_, jitterAmount_),
					rnd.NextFloatRange(-jitterAmount_, jitterAmount_)
				};
				jitteredCorners[j] = corners3D[j] + jitter;
			}

			TOMATOsEngine::DrawLine3D(jitteredCorners[0], jitteredCorners[1], colorInt);
			TOMATOsEngine::DrawLine3D(jitteredCorners[1], jitteredCorners[2], colorInt);
			TOMATOsEngine::DrawLine3D(jitteredCorners[2], jitteredCorners[3], colorInt);
			TOMATOsEngine::DrawLine3D(jitteredCorners[3], jitteredCorners[0], colorInt);
		}
	}
}
