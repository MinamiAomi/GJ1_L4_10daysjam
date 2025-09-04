#include "Score.h"

#include "TOMATOsEngine.h"

#include "Wall.h"
#include "Border.h"
#include "HexagonSevenSegmentDisplay.h"

void Score::Initialize()
{
	score_ = Border::GetInstance()->GetBorderSidePos();
	preScore_ = score_;
	position_ = { 20.0f,20.0f };
	spacing_ = 4.5f;
	scale_ = 0.3f;
	color_ = 0x808080FF;
}

void Score::Update()
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
	preScore_ = score_;
	//大きいと更新
	if (score_ < Border::GetInstance()->GetBorderSidePos()) {
		score_ = Border::GetInstance()->GetBorderSidePos();
	}
	UpdateDrawVertex();
}

void Score::Draw() {
	static	const int verticesPerHexagon = 6;

	if (!drawVertex_.empty()) {
		if (score_ != preScore_) {
			color_ = 0xDCDCDCFF;
		}
		else {
			color_ = 0x808080FF;
		}
		for (size_t i = 0; i < drawVertex_.size(); i += verticesPerHexagon) {
			for (int j = 0; j < verticesPerHexagon; ++j) {
				Vector2 start = drawVertex_.at(i + j);
				Vector2 end = drawVertex_.at(i + (j + 1) % verticesPerHexagon);
				TOMATOsEngine::DrawLine3D({ start.x, start.y, 0.0f }, { end.x, end.y, 0.0f }, color_);
			}
		}
	}
}

void Score::UpdateDrawVertex()
{
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
