#pragma once

#include <vector>
#include <memory>

#include "ICollider.h"

class Player;
class CollisionManager {
public:
	static CollisionManager* GetInstance();

	void Initialize();

	void SetPlayer(Player* player) { player_ = player; }

	void Update();

	void AddCollision(ICollider* collider) { colliders_.push_back(collider); }
private:
	bool IsSphereCollision(const Vector2& posA,float radiusA, const Vector2& posB, float radiusB);
	bool IsSquareCollision(const Vector2& posA,float radiusA, const Vector2& posB, float radiusB);

	std::vector<ICollider*> colliders_;
	Player* player_;
};