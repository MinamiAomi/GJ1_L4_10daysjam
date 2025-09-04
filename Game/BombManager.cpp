#include "BombManager.h"

#include "CollisionManager.h"

void BombManager::Initialize()
{
}

void BombManager::Update()
{
	for (auto& bomb : bombs_) {
		if (bomb->GetIsAlive()) {
			bomb->Update();
		}
	}

	bombs_.remove_if([](const std::unique_ptr<Bomb>& bomb) {
		return !bomb->GetIsAlive();
		});

	for (auto& bomb : bombs_) {
		if (bomb->GetIsAlive()) {
			CollisionManager::GetInstance()->AddCollision(bomb.get());
		}
	}
}

void BombManager::Draw()
{
	for (auto& bomb : bombs_) {
		if (bomb->GetIsAlive()) {
			bomb->Draw();
		}
	}
}

void BombManager::Spawn(const Vector2& position, float radius, int color)
{
	auto newBomb = std::make_unique<Bomb>();

	newBomb->Initialize(position, radius, color);

	bombs_.push_back(std::move(newBomb));
}
