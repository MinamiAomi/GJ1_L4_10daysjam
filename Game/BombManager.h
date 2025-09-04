#pragma once

#include <list>
#include <memory>

#include "Bomb.h"

class BombManager {
public:
	void Initialize();

	void Update();

	void Draw();

	void Spawn(const Vector2& position,float radius,int color);

	const std::list<std::unique_ptr<Bomb>>& GetBombs() const { return bombs_; }
private:
	std::list<std::unique_ptr<Bomb>> bombs_;
};