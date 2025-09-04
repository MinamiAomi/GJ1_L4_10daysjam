#pragma once

#include <array>
#include <vector>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Player;

class PlayerModel {
public:

	enum State {
		kIdle,
		kMove,
		kJump
	};


	void Initialize(const Player* player);
	void Update();
	void Draw();

	void SetState(State state) { state_ = state; };
	
private:
	const Player* player_;
	State state_;
	Square head_;
	Square leftFoot_;
	Square rightFoot_;
	Square leftEye_;
	Square rightEye_;

	float walkCycle_ = 0.0f; 
	float walkSpeed_ = 0.2f;         
	float walkAmplitude_ = 0.0f;

	float idleCycle_ = 0.0f;
	float idleSpeed_ = 0.2f;
	float idleAmplitude_ = 0.0f;

	Vector2 initialLeftFootPos_;
	Vector2 initialRightFootPos_;

	void AnimationInitialize();
	void Idle();
	void Move();
	void Jump();
};