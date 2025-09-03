#pragma once

#include <vector>

#include "TextureHandle.h"
#include "Math/MathUtils.h"

class Score {
public:
	void Initialize();

	void Update();

	void Draw();
private:
	float score_;
	float preScore_;

	Vector2 position_;
	
	//文字の間
	float spacing_;
	// 色
	int color_;
	//文字のサイズ
	float scale_;
	//描画する頂点
	std::vector<Vector2> drawVertex_;

	void UpdateDrawVertex();
};