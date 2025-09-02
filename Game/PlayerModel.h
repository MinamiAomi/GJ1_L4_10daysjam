#pragma once

#include <array>
#include <vector>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Player;

class PlayerModel {
public:

	enum State {
		Idle,
		Move,
		Jump
	};

	void Initialize(const Player* player);
	void Update();
	void Draw();
	
private:
	const Player* player_;
	
};