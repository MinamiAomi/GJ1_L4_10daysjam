#pragma once

#include <array>
#include <memory>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class BackGround {
private:
	

public:
	BackGround();
	~BackGround();
	void Initialize();
	void Update();
	void Draw();

	
private:
	float space_;
	float z_;
	float y_;
	float outerRadius_;
	float innerRadius_;
	float stemHeight_;
	float time_;
	float addTime_;
};

