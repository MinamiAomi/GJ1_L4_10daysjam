#pragma once

#include "Math/MathUtils.h"

class ICollider {
public:
	virtual void OnCollision(const Vector2& position,float radius) = 0;
	virtual void OnPlayerHitCollision() = 0;

	const Vector2& GetPosition() { return position_; }
	float GetRadius() { return radius_; }
protected:
	Vector2 position_;
	float radius_;
};