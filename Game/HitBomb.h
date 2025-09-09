#pragma once

#include "ICollider.h"

#include "Math/MathUtils.h"

class Player;

class HitBomb : public ICollider {
public:
	void Initialize(const Vector2& position, float radius, int color);

	void Update();

	void Draw();

	bool GetIsAlive() { return isAlive_; }

	int GetColor() { return color_; }

	void Despawn();
private:
	void OnCollision(const Vector2& position, float radius)override;
	void OnPlayerHitCollision(Player* player)override;

	bool isAlive_;
	int color_;
};