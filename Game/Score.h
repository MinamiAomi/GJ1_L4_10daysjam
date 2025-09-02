#pragma once

#include <array>

#include "TextureHandle.h"
#include "Math/MathUtils.h"

class Score {
public:
	void Initialize();

	void Update();

	void Draw();
private:
	float score_;
	//文字の間
	float spacing_;
	//文字サイズ
	float scale_;
	Vector2 position_;
	//float comboAcceptanceTime_;
};