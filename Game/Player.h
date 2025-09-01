#pragma once

#include <array>
#include <vector>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Field;
class ParticleManager;

class Player {
public:

	void Initialize();
	void Update();
	void Draw();

	void SetPosition(const Vector2& position) { position_ = position; }
	
private:
	static const int32_t kAnimationSwitchNum = 10;


	// 真ん中
	Vector2 position_;
	Vector2 size_;

	Vector2 velocity_;
};