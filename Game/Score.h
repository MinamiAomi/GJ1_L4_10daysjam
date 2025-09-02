#pragma once

#include <array>

#include "TextureHandle.h"
#include "Math/MathUtils.h"

class Score {
public:
	Score();

	void Initialize();

	void Update();

	void Draw();

private:
	float bombRank_;
	
	
	
};