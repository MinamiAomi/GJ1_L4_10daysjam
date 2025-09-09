#include "HitBomb.h"

#include "TOMATOsEngine.h"

#include "Player.h"
#include "CollisionManager.h"
#include "Particle/ParticleManager.h"
#include "Wall.h"
#include "Border.h"

#include "Math/Color.h"

void HitBomb::Initialize(const Vector2& position, float radius, int color)
{
	position_ = position;
	radius_ = radius;
	color_ = color;
	isAlive_ = true;
}

void HitBomb::Update()
{
	const auto& wall = Wall::GetInstance();
	//wallに触れたら
	if (position_.x - radius_ < wall->GetPosition()) {
		position_.x = wall->GetPosition() + radius_;
	}
}

void HitBomb::Draw()
{
	TOMATOsEngine::DrawBoxLine3D(position_, { radius_,radius_ }, color_);
}

void HitBomb::OnCollision(const Vector2& position, float radius)
{
	position;
	radius;
	////自分が当たってるやつより左にいるときだけ押し戻し
	//if (position_.x > position.x) {
	//	position_.x = position.x + radius;
	//}
}

void HitBomb::OnPlayerHitCollision(Player* player)
{
	ParticleManager::GetInstance()->GetPop()->Create(position_, Color::Convert(color_), 10);
	isAlive_ = false;

	//ヒップドロップしているかどうか
	if (player->GetIsHipDrop()) {
		Border::GetInstance()->PushBackHipDrop(1);
	}
	else {
		Border::GetInstance()->PushBack(-1);
	}
}