#include "Score.h"

#include "TOMATOsEngine.h"

#include "Border.h"
#include "HexagonSevenSegmentDisplay.h"
void Score::Initialize()
{
	score_ = 0.0f;
	position_ = { -15.0f,10.0f };
	spacing_ = 4.0f;
	scale_ = 0.4f;
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
	//大きいと更新
	if (score_ < Border::GetInstance()->GetBorderLinePos()) {
		score_ = Border::GetInstance()->GetBorderLinePos();
	}
}

void Score::Draw() {

	// 1. スコアを "0000.00" 形式の文字列にフォーマットする
	char buffer[10]; // "0000.00" + 終端文字で十分なサイズ
	// C++11以降でより安全な snprintf を使用
	snprintf(buffer, sizeof(buffer), "%07.2f", score_);
	std::string scoreStr(buffer);

	// 2. 文字列を1文字ずつ描画していく
	
	Vector2 currentPosition = position_ ;
	for (char c : scoreStr) {
		std::vector<Vector2> vertices;

		if (c == '.') {
			// 3a. 小数点の場合: 'g'セグメント(中央の横棒)を下にずらして描画する
			Vector2 dpPosition = currentPosition;
			dpPosition.y -= scale_ * 6.0f; // 'd'セグメントの位置を参考に下にずらす

			// GenerateSegmentVertices はクラス外の関数として前回作成したもの
			vertices = HexagonSevenSegmentDisplay::GetInstance()->GenerateSegmentVertices('g', { 0, 0 }); // まず原点中心で生成

			// 手動で平行移動とスケーリングを行う
			for (auto& v : vertices) {
				v.x *= scale_;
				v.y *= scale_;
				v = v + dpPosition;
			}

		}
		else if (isdigit(c)) {
			// 3b. 数字の場合
			int digit = c - '0'; // 文字を整数に変換
			vertices = HexagonSevenSegmentDisplay::GetInstance()->GetNumberVertex(digit, currentPosition);

			// 取得した頂点をスケーリングし、現在の描画位置に平行移動させる
			for (auto& v : vertices) {
				// まず原点周りでスケーリング
				v.x *= scale_;
				v.y *= scale_;
				// 次に描画位置へ平行移動
				v = v + currentPosition;
			}
		}

		// 4. 取得・加工した頂点を使って六角形を描画する
		if (!vertices.empty()) {
			const int verticesPerHexagon = 6;
			for (size_t i = 0; i < vertices.size(); i += verticesPerHexagon) {
				for (int j = 0; j < verticesPerHexagon; ++j) {
					const Vector2& start = vertices.at(i + j);
					const Vector2& end = vertices.at(i + (j + 1) % verticesPerHexagon);
					TOMATOsEngine::DrawLine3D(start, end, 0x0000FFFF);
				}
			}
		}

		// 5. 次の文字の描画位置へ移動
		currentPosition.x += spacing_;
	}
}