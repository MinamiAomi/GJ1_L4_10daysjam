#pragma once

#include "Math/MathUtils.h"

class Player;

class ICollider {
public:
	virtual ~ICollider() = default; 
	virtual void OnCollision(const Vector2& position,float radius) = 0;
	virtual void OnPlayerHitCollision(Player* player) = 0;

	const Vector2& GetPosition() { return position_; }
	float GetRadius() { return radius_; }
protected:
	Vector2 position_;
	float radius_;
};