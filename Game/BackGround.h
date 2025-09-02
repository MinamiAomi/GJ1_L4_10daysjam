#pragma once

#include <array>
#include <memory>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Player;
class BackGround {
private:
	

public:
	BackGround();
	~BackGround();
	void Initialize();
	void Update();
	void Draw();

	void SetPlayer(Player* player) { player_ = player; }
	
private:
	// フレーム
	TextureHandle fream_TextureHandle_;
	Vector2 fream_Position_;
	Vector2 fream_Size_;
	float fream_ColorH_;
	Player* player_;
};

