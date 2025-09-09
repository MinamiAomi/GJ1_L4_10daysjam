#pragma once

#include <list>
#include <memory>

#include "Bomb.h"
#include "HitBomb.h"

class BombManager {
public:
	void Initialize();

	void Update();

	void Draw();

	void SpawnBomb(const Vector2& position,float radius,int color);
	void SpawnHitBomb(const Vector2& position,float radius,int color);

	void AllDespawnBomb();

	const std::list<std::unique_ptr<Bomb>>& GetBombs() const { return bombs_; }
	const std::list<std::unique_ptr<HitBomb>>& GetHitBombs() const { return hitBombs_; }
private:
	std::list<std::unique_ptr<Bomb>> bombs_;
	std::list<std::unique_ptr<HitBomb>> hitBombs_;
};