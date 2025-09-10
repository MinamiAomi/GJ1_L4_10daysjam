#pragma once

#include <array>
#include <memory>

#include "Math/MathUtils.h"
#include "Math/Random.h"
#include "TextureHandle.h"

class BackGround {
private:
	static const int kParticleNum = 50;

public:

	enum ParticleType {
		kStar,
		kSquare,
		kTriangle,
		kPlus
	};

	BackGround();
	~BackGround();
	void Initialize();
	void Update();
	void Draw();
	void ResultDraw();

	
private:
	float space_;
	float z_;
	float y_;
	float outerRadius_;
	float innerRadius_;
	float stemHeight_;
	float time_;
	float addTime_;
	float adOffset_;

	Vector2 moonOffset_;
	float moonThickness_;
	float moonRotation_;
	float moonOuterRadius_;

	int emitFrame_;
	int spaceEmitFrame_;
	Vector2 particlePos_[kParticleNum];
	bool isActiveParticle_[kParticleNum];
	ParticleType particleType_[kParticleNum];
	float particleRadian_[kParticleNum];
	float particleSpeed_ = 0.1f;
	float particleRotateSpeed_ = 1 * Math::ToRadian;
	Random::RandomNumberGenerator random_;
};

