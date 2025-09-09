#pragma once

#include <vector>

#include "Math/MathUtils.h"
#include "Math/Color.h"

class Score {
public:
	void Initialize();

	void Update();

	void Draw();

	void SetPosition(const Vector2& pos) { position_ = pos; }
private:
	float score_;

	Vector2 position_;
	
	//文字の間
	float spacing_;
	// 色
	Color color_;
	Color defaultColor_;
	Color burstColor_;

	//文字のサイズ
	float scale_;
	//描画する頂点
	std::vector<Vector2> drawVertex_;

	void UpdateDrawVertex();
};