#pragma once

#include <array>
#include <memory>

#include "Math/MathUtils.h"
#include "Math/Random.h"
#include "TextureHandle.h"

class BackGround {
private:
	static const int kStarNum_ = 50;

public:
	BackGround();
	~BackGround();
	void Initialize();
	void Update();
	void Draw();
	void ResultDraw();

	
private:
	float space_;
	float z_;
	float y_;
	float outerRadius_;
	float innerRadius_;
	float stemHeight_;
	float time_;
	float addTime_;
	float adOffset_;

	Vector2 moonOffset_;
	float moonThickness_;
	float moonRotation_;
	float moonOuterRadius_;

	Vector2 starPosOffsets_[kStarNum_];
	Square starSquare_;
	Random::RandomNumberGenerator random_;
};

