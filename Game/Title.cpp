#include "Title.h"

#include "TOMATOsEngine.h" 
#include "Math/Color.h"
#include "Easing.h"

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
		titleText_.Update(t);
		if (animationTime_ >= kAnimationDuration) {
			state_ = title;
			animationTime_ = 0.0f;
		}
	}
	break;
	case Title::title:
	{
		//animationTime_ += 1.0f / 60.0f;

		//animationTime_ = std::fmodf(animationTime_, kAnimationDuration);

		//float t = animationTime_ / kAnimationDuration;

		////titleText_.Update(t);
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
}

void Title::TitleText::Update(float animationTime)
{
#ifdef _DEBUG
	ImGui::Begin("Title Settings");
	ImGui::DragFloat2("Position", &position_.x, 1.0f);
	ImGui::DragFloat("Scale", &scale_, 1.0f, 10.0f, 500.0f);
	ImGui::DragFloat("Space", &space_, 1.0f, 10.0f, 500.0f);
	ImGui::DragFloat("Thickness", &outlineThickness_, 0.001f, 0.0f, 0.5f);
	ImGui::End();
#endif // _DEBUG
	scale_ = Easing::easing(Easing::InOutBack(animationTime), kScale * 500.0f, kScale);
}

void Title::TitleText::Draw()
{// 文字の真ん中探し
	int numCharacters = static_cast<int>(titleText_.size());
	if (numCharacters == 0) return;
	float totalWidth = (numCharacters - 1) * space_;
	Vector2 drawingStartPosition = position_;
	drawingStartPosition.x -= totalWidth * 0.5f;

	Vector2 currentCharacterPosition = drawingStartPosition;

	// 文字ごとのループ
	for (const auto& character : titleText_) {
		for (size_t i = 0; i < character.size(); i += 2) {
			// スケール、位置を適応
			Vector2 start = (character.at(i) * scale_) + currentCharacterPosition;
			Vector2 end = (character.at(i + 1) * scale_) + currentCharacterPosition;

			// 垂直を調べる
			Vector2 dir = end - start;
			// ゼロ除算を避ける
			if (dir.LengthSquare() == 0.0f) continue;
			dir.Normalize();
			Vector2 perp = { -dir.y, dir.x };

			// 輪郭に合わせてスケール
			float thickness = outlineThickness_ * scale_;

			// 太さを持たせた四角形
			Vector2 halfPerp = perp * (thickness / 2.0f);
			Vector2 c1 = start + halfPerp;
			Vector2 c2 = end + halfPerp;
			Vector2 c3 = end - halfPerp;
			Vector2 c4 = start - halfPerp;

			TOMATOsEngine::DrawLine3D({ c1.x, c1.y, 0.0f }, { c2.x, c2.y, 0.0f }, Color(color_));
			TOMATOsEngine::DrawLine3D({ c2.x, c2.y, 0.0f }, { c3.x, c3.y, 0.0f }, Color(color_));
			TOMATOsEngine::DrawLine3D({ c3.x, c3.y, 0.0f }, { c4.x, c4.y, 0.0f }, Color(color_));
			TOMATOsEngine::DrawLine3D({ c4.x, c4.y, 0.0f }, { c1.x, c1.y, 0.0f }, Color(color_));
		}

		// 次の文字の中心位置へ移動
		currentCharacterPosition.x += space_;
	}
}
