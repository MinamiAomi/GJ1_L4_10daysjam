#include "BombManager.h"

void BombManager::Initialize()
{
}

void BombManager::Update()
{
	for (auto& bomb : bombs_) {
		bomb.Update();
	}
}

void BombManager::Draw()
{
	for (auto& bomb : bombs_) {
		bomb.Draw();
	}
}

void BombManager::Spawn(const Vector2& position, float radius, int color)
{
	Bomb bomb;
	bomb.Initialize(position, radius, color);
	bombs_.push_back(bomb);
}

void BombManager::Despawn(const Bomb& bomb)
{
	bombs_.remove_if([&](const Bomb& list_bomb) {
		return &list_bomb == &bomb;
		});
}

