#pragma once

#include "Math/MathUtils.h"

class Player;
class Border {
public:
	const float kWallWidth = 3.0f;
	const float kWallHeight = 50.0f;

	static Border* GetInstance();

	void Initialize();

	void Update();

	void Draw();

	/// <summary>
	/// 0->0爆弾
	/// 1->1爆弾
	/// 2->2爆弾
	/// </summary>
	/// <param name="add"></param>
	void PushBack(int add);

	void PushBackHipDrop(int add);

	bool IsMove() { return pushBackPosition_ != 0.0f; }
	//現在の横BorderLinePos
	float GetBorderSidePos();
	/// <summary>
	/// ゲームスタート時のPosition(俺以外使わないと思う)
	/// </summary>
	/// <returns></returns>
	float GetBorderFirstPos();

	//ボーダーが移動する
	float GetPushBackPosition();

	void SetPlayer(Player* player) { player_ = player; }
private:
	//ボムの計算
	void CalcBomb();

	//手前のライン
	float position_;
	//押し戻しのポジション
	float pushBackPosition_;
	//イージングの速度
	float easingSpeed_;
	//押し戻し係数
	float pushBackCoefficient_;
	float pushBackHipDropCoefficient_;
	//float comboCoefficient_;
	//
	////コンボ継続時間
	//float comboDuration_;
	//float comboTime_;

	int color_;
	float firstPosition_;

	int hitBombNum_;

	Player* player_;
};