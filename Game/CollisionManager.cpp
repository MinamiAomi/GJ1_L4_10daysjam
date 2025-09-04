#include "CollisionManager.h"

#include "Player.h"

CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::Initialize()
{
	colliders_.clear();
}

void CollisionManager::Update()
{
	if (!colliders_.empty()) {
		// コライダー同士の総当たり判定
		for (size_t i = 0; i < colliders_.size(); ++i) {
			for (size_t j = i + 1; j < colliders_.size(); ++j) {
				auto& colliderA = colliders_[i];
				auto& colliderB = colliders_[j];

				if (IsSquareCollision(colliderA->GetPosition(), colliderA->GetRadius(), colliderB->GetPosition(), colliderB->GetRadius())) {
					colliderA->OnCollision(colliderB->GetPosition(), colliderB->GetRadius());
					colliderB->OnCollision(colliderA->GetPosition(), colliderA->GetRadius());
				}
			}
		}

		// プレイヤーと各コライダーの当たり判定
		for (const auto& colliderA : colliders_) {
			if (IsSphereCollision(colliderA->GetPosition(), colliderA->GetRadius() * 0.5f, player_->GetPosition(), player_->GetSize().x)) {
				
				colliderA->OnPlayerHitCollision();
				player_->AddHitBom(); 
			}
		}

		colliders_.clear();
	}

}

bool CollisionManager::IsSphereCollision(const Vector2& posA, float radiusA, const Vector2& posB, float radiusB)
{
	float a = posA.x - posB.x;
	float b = posA.y - posB.y;
	float c = std::sqrt(a * a + b * b);
	if (c <= radiusA + radiusB) {
		return true;
	}

	return false;
}

bool CollisionManager::IsSquareCollision(const Vector2& posA, float radiusA, const Vector2& posB, float radiusB)
{
	struct Square {
		Square(const Vector2& pos, float radius) {
			float harfRadius = radius * 0.5f;
			min = { pos.x - harfRadius , pos.y - harfRadius };
			max = { pos.x + harfRadius , pos.y + harfRadius };
		}
		Vector2 min;
		Vector2 max;
	};

	Square a(posA, radiusA);
	Square b(posB, radiusB);

	if (a.max.x >= b.min.x && a.min.x <= b.max.x &&
		a.max.y >= b.min.y && a.min.y <= b.max.y)
	{
		return true;
	}

	return false;
}
