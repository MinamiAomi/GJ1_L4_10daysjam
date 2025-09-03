#pragma once

#include "Math/MathUtils.h"

class Bomb {
public:
	void Initialize(const Vector2& position,float radius,int color);

	void Update();

	void Draw();

	const Vector2& GetPosition() { return position_; }
	float GetRadius() { return radius_; }
	int GetColor() { return color_; }
private:
	Vector2 position_;
	float radius_;
	int color_;
};