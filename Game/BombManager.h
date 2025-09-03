#pragma once

#include <List>

#include "Bomb.h"

class BombManager {
public:
	void Initialize();

	void Update();

	void Draw();

	void Spawn(const Vector2& position,float radius,int color);
	
	void Despawn(const Bomb& bomb);

	std::list<Bomb>& GetBombs() { return bombs_; }
private:
	std::list<Bomb> bombs_;
};