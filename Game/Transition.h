#pragma once

enum GameScene {
	title,
	inGame,
	gameClear,
};

class Transition
{
public:

	static Transition* GetInstance();

	void Start(GameScene preScene) {
		isStart = true;
		pre = preScene;
	}
	void Update();

	void TransitionShake();

	bool isStart = false;

	float t = 0.0f;
	float t2 = 0.0f;
	float maxShakeValue = 90.0f;
	float speed = 0.02f;
	bool isNextSceneFrame = false;
	bool endPreScene = false;
	bool isPreScene = false;
	bool isNextScene = false;

	GameScene pre;
};

