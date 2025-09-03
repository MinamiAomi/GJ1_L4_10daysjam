#include "Bomb.h"

#include "TOMATOsEngine.h"

#include "Wall.h"

void Bomb::Initialize(const Vector2& position, float radius, int color)
{
	position_ = position;
	radius_ = radius;
	color_ = color;
}

void Bomb::Update()
{
	const auto& wall = Wall::GetInstance();
	if (position_.x - radius_ < wall->GetPosition()) {
		position_.x = wall->GetPosition()+ radius_;
	}
}

void Bomb::Draw()
{
	TOMATOsEngine::DrawBoxLine3D(position_, { radius_,radius_ }, color_);

}
