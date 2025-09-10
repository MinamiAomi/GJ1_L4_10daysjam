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
		kJump,
		kWallSliding,
		kHipDrop,
		kEndHipDrop //1frameしか入らない
	};


	void Initialize(const Player* player);
	void Update();
	void Draw();
	void ResultDraw();

	void SetState(State state) { state_ = state; };
	
private:
	const Player* player_;
	State state_;
	State preState_;
	Square head_;
	Square leftFoot_;
	Square rightFoot_;
	Square leftEye_;
	Square rightEye_;

	float changeT_ = 0.0f;

	float walkCycle_ = 0.0f; 
	float walkSpeed_ = 0.2f;         
	float walkAmplitude_ = 0.0f;

	float idleCycle_ = 0.0f;
	float idleSpeed_ = 0.1f;
	float idleAmplitude_ = 0.0f;

	Square initialLeftFoot_;
	Square initialRightFoot_;
	Square initialHead_;

	Square jumpHead_;
	Square endHipDropSquare_;

	float offsetY = 0.3f;

	void Idle();
	void Move();
	void Jump();
	void WallSliding();
	void HipDrop();
	void EndHipDrop();
};