#include "Score.h"

#include "TOMATOsEngine.h"

#include "Wall.h"
#include "Border.h"
#include "HexagonSevenSegmentDisplay.h"

Score* Score::GetInstance()
{
	static Score score;
	return &score;
}

void Score::Initialize()
{
	score_ = Border::GetInstance()->GetBorderSidePos();
	position_ = { 20.0f,20.0f };
	spacing_ = 4.5f;
	scale_ = 0.3f;
	defaultColor_ = Color(0x808080FF);
	burstColor_ = Color(0xFFd700FF);
	color_ = defaultColor_;

	orbitAngle_ = 0.0f;
	orbitSpeed_ = 0.5f;
	orbitRadius_ = 250.0f;
}

void Score::Update(bool isTitle)
{
#ifdef _DEBUG
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("Score")) {
		ImGui::DragFloat2("Position", &position_.x, 0.01f);
		ImGui::DragFloat("Spacing", &spacing_, 1.0f);
		ImGui::DragFloat("Scale", &scale_, 0.1f);
		ImGui::DragFloat("Score", &score_, 0.1f);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
	drawVertex_.clear();
	if (!isTitle) {
		if (Border::GetInstance()->GetPushBackPosition() - Border::GetInstance()->GetBorderSidePos() > 0.0f) {
			color_ = burstColor_;
		}
		else {
			color_ = defaultColor_;
		}
		////カラーイージング
		//float wallPos = Wall::GetInstance()->GetPosition();
		//float borderPos = Border::GetInstance()->GetBorderSidePos();
		//float t = std::clamp((borderPos - wallPos) / Wall::kBurstDistance, 0.0f, 1.0f);

		//float r = std::lerp(defaultColor_.GetR(), burstColor_.GetR(), t);
		//float g = std::lerp(defaultColor_.GetG(), burstColor_.GetG(), t);
		//float b = std::lerp(defaultColor_.GetB(), burstColor_.GetB(), t);
		//float a = std::lerp(defaultColor_.GetA(), burstColor_.GetA(), t);

		//Color finalColor = Color::RGBA(r, g, b, a);
		//color_ = finalColor;

		//大きいと更新
		if (score_ < Border::GetInstance()->GetBorderSidePos()) {
			score_ = Border::GetInstance()->GetBorderSidePos();
		}
	}
	else {
		/*orbitAngle_ += orbitSpeed_ * (1.0f / 60.0f);
		orbitAngle_ = std::fmodf(orbitAngle_, 2.0f * Math::Pi);*/

		color_ = Color::white;
	}
	UpdateDrawVertex(isTitle);
}

void Score::Draw() {
	static	const int verticesPerHexagon = 6;

	if (!drawVertex_.empty()) {
		for (size_t i = 0; i < drawVertex_.size(); i += verticesPerHexagon) {
			for (int j = 0; j < verticesPerHexagon; ++j) {
				Vector2 start = drawVertex_.at(i + j);
				Vector2 end = drawVertex_.at(i + (j + 1) % verticesPerHexagon);
				TOMATOsEngine::DrawLine3D({ start.x, start.y, 0.0f }, { end.x, end.y, 0.0f }, color_);
			}
		}
	}
}

void Score::UpdateDrawVertex(bool isTitle)
{
	isTitle;
	float score = score_ - Border::GetInstance()->GetBorderFirstPos();
	// スコアを "0000.00" 形式に
	char buffer[10];
	snprintf(buffer, sizeof(buffer), "%07.2f", score);
	std::string scoreStr(buffer);

	if (scoreStr.length() == 0) {
		return;
	}

	// 文字列の長さを計算
	const float totalWidth = (scoreStr.length() - 1) * spacing_;

	//wallが親
	Vector2 worldPos = position_ + Vector2(Wall::GetInstance()->GetPosition(), 0.0f);
	//描画開始位置を総幅の半分だけ左にずらす
	Vector2 drawingStartPosition = worldPos;
	drawingStartPosition.x -= totalWidth * 0.5f;

	//各文字の中心
	Vector2 currentCharacterPosition = drawingStartPosition;

	for (char c : scoreStr) {
		if (c == '.') {
			// 小数点専用の小さな六角形
			const std::vector<Vector2> baseDecimalVertices = {
				{-1, 0}, {-0.5, 1}, {0.5, 1}, {1, 0}, {0.5, -1}, {-0.5, -1}
			};

			Vector2 dpPosition = currentCharacterPosition;
			// Y軸オフセット
			dpPosition.y -= 1.5f;

			// スケールと移動を行う
			for (const auto& v : baseDecimalVertices) {
				drawVertex_.push_back(Vector2({ v.x * scale_, v.y * scale_ }) + dpPosition);
			}

		}
		else if (isdigit(c)) {
			int digit = c - '0';
			std::vector<Vector2> baseVertices = HexagonSevenSegmentDisplay::GetInstance()->GetNumberVertex(digit);

			// スケールと移動を行う
			for (const auto& v : baseVertices) {
				drawVertex_.push_back(Vector2({ v.x * scale_, v.y * scale_ }) + currentCharacterPosition);
			}
		}

		//次の文字の描画中心位置へ移動
		currentCharacterPosition.x += spacing_;
	}
}
