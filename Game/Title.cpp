#include "Title.h"

#include <array>

#include "TOMATOsEngine.h" 
#include "Math/Color.h"
#include "Easing.h"

#include "Math/Random.h"

void Title::Initialize()
{
	titleText_.Initialize();
	animationTime_ = 0.0f;
}

void Title::Update()
{

	switch (state_)
	{
	case Title::first:
	{
		animationTime_ += 1.0f / 60.0f;
		float t = std::clamp(animationTime_ / kAnimationDuration, 0.0f, 1.0f);
		titleText_.AnimateFirst(t);
		if (animationTime_ >= kAnimationDuration) {
			state_ = title;
			animationTime_ = 0.0f;
		}
	}
	break;
	case Title::title:
	{
		animationTime_ += 1.0f / 60.0f;
		titleText_.AnimateTitleLoop();
		//float t = std::clamp(animationTime_ / kAnimationDuration, 0.0f, 1.0f);
	}
	break;
	case Title::ingame:
	{

	}
	break;
	default:
		break;
	}
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
	rotateSpeed_ = 0.5f;
	rotateRadius_ = 20.0f;
	jitterAmount_ = 0.5f;
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
	ImGui::DragFloat("JitterAmount", &jitterAmount_, 0.1f,0.0f);
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

	// ★★★ ここからが新しい計算ロジック ★★★

	// 1. 弧の長さ(space_)と半径(orbitRadius_)から、文字ごとの角度の間隔を計算
	//    半径が0に近い場合は0除算を避ける
	if (std::abs(rotateRadius_) < 0.1f) {
		// 半径がほぼ0の場合は、以前の平面的な描画などに切り替えることも可能
		// ここでは何もしないことで描画をスキップ
		return;
	}
	const float angleStep = space_ / rotateRadius_;

	// 2. 文字列全体の弧の長さを計算し、中央揃えのための開始角度を決定
	const float totalArcLength = (numCharacters - 1) * space_;
	const float totalArcAngle = totalArcLength / rotateRadius_;
	// 公転の基準角度から、全体の角度の半分だけ戻った位置が描画開始角度
	const float startAngle = rotateAngle_ - (totalArcAngle / 2.0f);

	// ★★★ 計算ロジックの変更はここまで ★★★

	// 3. 文字ごとのループ (インデックスが必要なため、通常のforループを使用)
	for (int charIndex = 0; charIndex < numCharacters; ++charIndex) {
		const auto& characterData = titleText_[charIndex];

		// 4. この文字の現在の公転角度を計算
		float charOrbitAngle = startAngle + (static_cast<float>(charIndex) * angleStep);

		// 5. 角度と半径から、この文字の3D空間での中心位置を計算
		Vector3 charCenterPosition;
		charCenterPosition.x = position_.x + rotateRadius_ * std::cosf(charOrbitAngle);
		charCenterPosition.y = position_.y;
		charCenterPosition.z = rotateRadius_ * std::sinf(charOrbitAngle);

		// 6. 各文字の線分を描画 (この中のロジックは変更なし)
		for (size_t i = 0; i < characterData.size(); i += 2) {
			// 輪郭計算 (原点中心で行う)
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
				// 2Dの輪郭頂点を、計算した3Dの公転位置へ移動させる
				corners3D[j] = {
					corners2D[j].x + charCenterPosition.x,
					corners2D[j].y + charCenterPosition.y,
					charCenterPosition.z
				};
			}

			// 揺れ（ジッター）を追加
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

//void Title::TitleText::Draw()
//{
//	Random::RandomNumberGenerator rnd{};
//	uint32_t colorInt = Color(color_);
//
//	int numCharacters = static_cast<int>(titleText_.size());
//	if (numCharacters == 0) return;
//
//	// ★★★ 公転アニメーションの計算 ★★★
//
//	// 1. 各文字を円周上に均等に配置するための、文字ごとの角度差
//	const float angleStep = (2.0f * Math::Pi) / static_cast<float>(numCharacters);
//
//	// 2. 文字ごとのループ (インデックスが必要なため、通常のforループを使用)
//	for (int charIndex = 0; charIndex < numCharacters; ++charIndex) {
//		const auto& characterData = titleText_[charIndex];
//
//		// 3. この文字の現在の公転角度を計算
//		float charOrbitAngle = rotateAngle_ + (static_cast<float>(charIndex) * angleStep);
//
//		// 4. 角度と半径から、この文字の3D空間での中心位置を計算
//		Vector3 charCenterPosition;
//		charCenterPosition.x = position_.x + rotateRadius_ * std::cosf(charOrbitAngle);
//		charCenterPosition.y = position_.y; // Yはずっと同じ高さ
//		charCenterPosition.z = rotateRadius_ * std::sinf(charOrbitAngle);
//
//		// 5. 各文字の線分を描画
//		for (size_t i = 0; i < characterData.size(); i += 2) {
//			// 輪郭計算 (原点中心で行う)
//			Vector2 start2D = characterData[i] * scale_;
//			Vector2 end2D = characterData[i + 1] * scale_;
//			// ... (輪郭計算のコードは長いため省略、変更なし) ...
//			Vector2 dir2D = end2D - start2D;
//			if (dir2D.LengthSquare() == 0.0f) continue;
//			dir2D.Normalize();
//			Vector2 perp2D = { -dir2D.y, dir2D.x };
//			float thickness = outlineThickness_ * scale_;
//			Vector2 halfPerp = perp2D * (thickness / 2.0f);
//			std::array<Vector2, 4> corners2D = {
//				start2D + halfPerp, end2D + halfPerp,
//				end2D - halfPerp, start2D - halfPerp
//			};
//
//			std::array<Vector3, 4> corners3D;
//			for (int j = 0; j < 4; ++j) {
//				// 6. 2Dの輪郭頂点を、計算した3Dの公転位置へ移動させる
//				//    文字自体は回転させないので、常にカメラを向く
//				corners3D[j] = {
//					corners2D[j].x + charCenterPosition.x,
//					corners2D[j].y + charCenterPosition.y,
//					charCenterPosition.z
//				};
//			}
//
//			// 7. 揺れ（ジッター）を追加
//			std::array<Vector3, 4> jitteredCorners;
//			for (int j = 0; j < 4; ++j) {
//				Vector3 jitter = {
//					rnd.NextFloatRange(-jitterAmount_, jitterAmount_),
//					rnd.NextFloatRange(-jitterAmount_, jitterAmount_),
//					rnd.NextFloatRange(-jitterAmount_, jitterAmount_)
//				};
//				jitteredCorners[j] = corners3D[j] + jitter;
//			}
//
//			TOMATOsEngine::DrawLine3D(jitteredCorners[0], jitteredCorners[1], colorInt);
//			TOMATOsEngine::DrawLine3D(jitteredCorners[1], jitteredCorners[2], colorInt);
//			TOMATOsEngine::DrawLine3D(jitteredCorners[2], jitteredCorners[3], colorInt);
//			TOMATOsEngine::DrawLine3D(jitteredCorners[3], jitteredCorners[0], colorInt);
//		}
//	}
//}
