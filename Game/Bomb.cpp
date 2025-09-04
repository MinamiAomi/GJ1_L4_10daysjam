#include "Bomb.h"

#include "TOMATOsEngine.h"

#include "CollisionManager.h"
#include "Particle/ParticleManager.h"
#include "Wall.h"

#include "Math/Color.h"

void Bomb::Initialize(const Vector2& position, float radius, int color)
{
	position_ = position;
	radius_ = radius;
	color_ = color;
	isAlive_ = true;
}

void Bomb::Update()
{
	const auto& wall = Wall::GetInstance();
	//wallに触れたら
	if (position_.x - radius_ < wall->GetPosition()) {
		position_.x = wall->GetPosition() + radius_;
	}
}

void Bomb::Draw()
{
	TOMATOsEngine::DrawBoxLine3D(position_, { radius_,radius_ }, color_);
}

void Bomb::OnCollision(const Vector2& position, float radius)
{
	//自分が当たってるやつより左にいるときだけ押し戻し
	if (position_.x > position.x) {
		position_.x = position.x + radius;
	}
}

void Bomb::OnPlayerHitCollision()
{
	ParticleManager::GetInstance()->GetPop()->Create(position_, { 1.0f,1.0f,1.0f,1.0 }, 10);
	isAlive_ = false;
}
