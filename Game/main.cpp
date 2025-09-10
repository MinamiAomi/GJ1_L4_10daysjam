#include <Windows.h>

#include "Engine/TOMATOsEngine.h"

#include "RenderManager.h"

#include "Math/Camera.h"

#include "Audio/Audio.h"

#include "Math/Color.h"
#include "Math/Animation.h"

//ゲームオブジェクトinclude
#include "Particle/ParticleManager.h"
#include "StageObjectManager.h"
#include "SpwanManager.h"
#include "CollisionManager.h"
#include "HitStopManager.h"

#include "BackGround.h"
#include "Player.h"
#include "Ground.h"
#include "Border.h"
#include "Score.h"
#include "Wall.h"
#include "Title.h"
#include "HexagonSevenSegmentDisplay.h"
#include "OperationInstructions.h"
#include "Transition.h"

#define INVALID_PLAY_HANDLE (size_t(-1))



int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {


	TOMATOsEngine::Initialize();
#ifdef _DEBUG
	TOMATOsEngine::SetFullScreen(false);
#endif // _DEBUG


	GameScene gameScene = title;

	Transition* transition = Transition::GetInstance();


	Vector3 cameraPosition = { 0.0f, 3.0f, -50.0f };
	Vector3 cameraRotate = {};
	Camera camera;
	camera.SetPosition({ 0.0f, 3.0f, -10.0f });

#pragma region テクスチャハンドル
	//シャットダウン
	TextureHandle shutdownTextureHandle = TOMATOsEngine::LoadTexture("Resources/shpere.png");
#pragma endregion

#pragma endregion

#pragma region 音
	//イワシロ音楽素材 使用の際は追記
	auto pushSpaceSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/pushSpace.wav");
	auto titleSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/titleBGM.wav");
	auto ingameSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/ingameBGM.wav");
	auto clearSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/clearBGM.wav");
	size_t pickHandle = TOMATOsEngine::LoadAudio("Resources/Audio/pick.wav");
	auto shutdownSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/shutdown.wav");
	const float bgmVolume = 0.2f;

	// タイトルははじめから流す
	size_t titlePlayHandle = TOMATOsEngine::PlayAudio(titleSoundHandle, true);
	TOMATOsEngine::SetVolume(titlePlayHandle, bgmVolume);
	size_t ingamePlayHandle = INVALID_PLAY_HANDLE;
	size_t clearPlayHandle = INVALID_PLAY_HANDLE;
	// 音の溜め必要
	bool ingameToClear = false;
	bool clearToTitle = false;
#pragma endregion

#pragma region シャットダウン
	// シャットダウンアニメーション
	float kMaxAnimationTime = 20.0f;
	float animationTime = 0.0f;
	bool isShutdown = false;


	Vector2 shutdownSize{};
	Vector2 shutdownSideSize{};
	Animation::Vector2Node shutdownSizeAnimation({ { {1280.0f, 20.0f}, 0.0f}, {{20.0f, 20.0f}, 0.2f}, {{20.0f, 640.0f}, 0.4f}, {{2.0f, 10.0f}, 0.4f}, {{1280.0f, 1.0f}, 1.0f} });
	Animation::Vector2Node shutdownSideSizeAnimation({ {{},0.0f}, { { 30.0f,100.0f }, 0.2f }, { {30.0f,200.0f} ,0.4f },{{},0.4f}, { {},1.0f } });;
#pragma endregion

#pragma region クラス宣言/初期化関連
	StageObjectManager* stageObjectManager = StageObjectManager::GetInstance();
	stageObjectManager->Initialize();

	SpawnManager* spawnManager = SpawnManager::GetInstance();
	spawnManager->Initialize();

	CollisionManager* collisionManager = CollisionManager::GetInstance();
	collisionManager->Initialize();

	Wall* wall = Wall::GetInstance();
	wall->Initialize();

	HitStopManager* hitStopManager = HitStopManager::GetInstance();
	hitStopManager->Initialize();

	Ground* ground = Ground::GetInstance();

	Player player;
	//初期化にWall使用
	Border* border = Border::GetInstance();
	border->Initialize();
	border->SetPlayer(&player);

	ParticleManager* particleManager = ParticleManager::GetInstance();
	particleManager->Initialize();

	player.Initialize();
	collisionManager->SetPlayer(&player);

	BackGround backGround;
	backGround.Initialize();

    OperationInstructions operationInstructions;

	Score* score = Score::GetInstance()->GetInstance();;
	score->Initialize();
	Vector2 titleScorePos = { 0.0f,-6.0f };
	score->SetPosition(titleScorePos);

	//変数名かぶり
	Title title_;
	title_.Initialize();
	//Test
	//HexagonSevenSegmentDisplay* hexagonSevenSegmentDisplay = HexagonSevenSegmentDisplay::GetInstance();
	//int hexagonSevenSegmentDisplayNumber = 0;
#pragma endregion

#pragma region まとめとく

	auto initializeInGame = [&]() {

		collisionManager->Initialize();
		stageObjectManager->Initialize();
		spawnManager->Initialize();
		hitStopManager->Initialize();

		wall->Initialize();
		border->Initialize();
		particleManager->Initialize();

		backGround.Initialize();
		player.Initialize();
		player.SetPosition({ 100.0f * 0.5f, 300.0f - 100.0f });
		score->Initialize();
		// 音
		// タイトルBGM停止
		TOMATOsEngine::StopAudio(titlePlayHandle);
		// インゲームBGM
		ingamePlayHandle = TOMATOsEngine::PlayAudio(ingameSoundHandle, true);
		TOMATOsEngine::SetVolume(ingamePlayHandle, bgmVolume);
		};

#pragma endregion

	while (TOMATOsEngine::BeginFrame()) {
		auto pad = TOMATOsEngine::GetGamePadState();
		auto prepad = TOMATOsEngine::GetGamePadPreState();
		const float commonShakeValue = 0.1f;
		TOMATOsEngine::SetLineShakeX(true, commonShakeValue);
		TOMATOsEngine::SetLineShakeY(true, commonShakeValue);
		transition->GetInstance()->Update();
#ifdef _DEBUG
		auto& io = ImGui::GetIO();
		ImGui::Begin("Menu");
		if (ImGui::TreeNode("Main")) {
			ImGui::Text("FPS : %f\n", io.Framerate);
			ImGui::Text("Quit : ESCAPE\n");
			ImGui::Text("now:%d", pad.Gamepad.sThumbLY);
			ImGui::Text("pre:%d", prepad.Gamepad.sThumbLY);
			ImGui::TreePop();
		}
		ImGui::End();
#endif // _DEBUG


		//カメラ処理がヒットストップで更新されないためここになりました皆さん気を付けてくださいな
		if (!hitStopManager->GetInstance()->GetIsHitStop()) {
			camera.SetPosition(cameraPosition);
			camera.SetRotate(Quaternion::MakeFromEulerAngle(cameraRotate * Math::ToRadian));
			camera.UpdateMatrices();
			TOMATOsEngine::SetCameraMatrix(camera.GetViewProjectionMatrix());
		}
		////////////////////////////////////////////////////更新////////////////////////////////////////////////////////

		float wallToBordarGap = border->GetBorderSidePos() - wall->GetPosition();
		float shakeStartDistance = 24.0f;
		float amplitude = 0.4f;
		float shakeValue = (shakeStartDistance - wallToBordarGap) / shakeStartDistance;

		switch (gameScene) {
		case title:
		{
			title_.Update();
			score->Update(true);
			//シャットダウン
			if (TOMATOsEngine::IsKeyTrigger(DIK_ESCAPE)) {
				isShutdown = true;
				TOMATOsEngine::PlayAudio(shutdownSoundHandle);
			}

			if (!isShutdown) {

#pragma region キー入力

				if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE) ||
					((pad.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
						!(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
					TOMATOsEngine::PlayAudio(pickHandle);

					transition->Start(title);
				}

				if (transition->isNextSceneFrame && transition->pre == title) {
					gameScene = inGame;
					initializeInGame();
				}
#pragma endregion



			}
			else {
				//シャットダウンアップデート
				animationTime += 1.0f;
				float t = animationTime / kMaxAnimationTime;
				shutdownSize = shutdownSizeAnimation.GetInterpolatedValue(t);
				shutdownSideSize = shutdownSideSizeAnimation.GetInterpolatedValue(t);

				if (animationTime >= kMaxAnimationTime) {
					TOMATOsEngine::RequestQuit();
				}
			}

			break;
		}

		case inGame:
		{
			hitStopManager->Update();

			//HitStop中止めたいところ入れ
			if (!hitStopManager->GetIsHitStop()) {
				player.Update();
				border->Update();
				stageObjectManager->Update();
				collisionManager->Update();
				wall->Update();
			}

			spawnManager->Update();

			backGround.Update();

			particleManager->Update();
			score->Update(false);


			//近づくとシェイク
			if (shakeValue >= 0.0f) {
				TOMATOsEngine::SetLineShakeX(true, shakeValue * amplitude);
				TOMATOsEngine::SetLineShakeY(true, shakeValue * amplitude);
			}

			//まけ判定
			if ( (wallToBordarGap <= 0.0f)  && !transition->isStart) {
				transition->Start(inGame);
				auto hitSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/hitHurt.wav");
				auto hitPlayHandle = TOMATOsEngine::PlayAudio(hitSoundHandle);

				TOMATOsEngine::SetVolume(hitPlayHandle, 1.0f);

				TOMATOsEngine::StopAudio(ingamePlayHandle);
				ingamePlayHandle = INVALID_PLAY_HANDLE;
			}

			if (transition->isNextSceneFrame && transition->pre == inGame) {
				gameScene = gameClear;
				TOMATOsEngine::StopAudio(ingamePlayHandle);
				ingamePlayHandle = INVALID_PLAY_HANDLE;
				score->SetPosition({ 0.0f, -5.0f });
			}

			if (TOMATOsEngine::IsKeyTrigger(DIK_ESCAPE)) {
				TOMATOsEngine::RequestQuit();

			}

			break;
		}
		case gameClear:
		{
			player.Update();
			backGround.Update();
			score->Update(true);


			if (!clearToTitle) {
				// ゲームクリアBGM
				clearPlayHandle = TOMATOsEngine::PlayAudio(clearSoundHandle, true);
				TOMATOsEngine::SetVolume(clearPlayHandle, bgmVolume);
				clearToTitle = true;
			}

			//タイトルに移動
			if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE) ||
				((pad.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
					!(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {

				transition->Start(gameClear);
			}


			if (transition->isNextSceneFrame && transition->pre == gameClear) {
				gameScene = title;

				score->SetPosition(titleScorePos);

				//初期化
				stageObjectManager->Initialize();
				spawnManager->Initialize();

				border->Initialize();
				wall->Initialize();
				particleManager->Initialize();

				backGround.Initialize();
				player.Initialize();

				title_.Initialize();


				// 音
				// クリアBGM停止
				TOMATOsEngine::StopAudio(clearPlayHandle);
				// タイトルBGM
				titlePlayHandle = TOMATOsEngine::PlayAudio(titleSoundHandle, true);
				TOMATOsEngine::SetVolume(titlePlayHandle, bgmVolume);


				// スペースオン
				auto pushSpacePlayHandle = TOMATOsEngine::PlayAudio(pushSpaceSoundHandle);
				TOMATOsEngine::SetVolume(pushSpacePlayHandle, 0.05f);
				clearToTitle = false;
				ingameToClear = false;
			}

			break;
		}

		default:
		{
			break;
		}
		}
		transition->TransitionShake();


		////////////////////////////////////////////////////更新////////////////////////////////////////////////////////
		switch (gameScene) {
		case title:
		{

			title_.Draw();
			score->Draw();

			if (isShutdown) {
				TOMATOsEngine::DrawSpriteRectAngle({ static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f }, { 1280.0f,1280.0f }, { 0.5f,0.5f }, 0.0f, {}, { 32.0f,32.0f }, shutdownTextureHandle, 0x000000FF);
				TOMATOsEngine::DrawSpriteRectAngle({ static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f }, shutdownSize, { 0.5f,0.5f }, 0.0f, {}, { 32.0f,32.0f }, shutdownTextureHandle, 0xFFFFFFFF);
				TOMATOsEngine::DrawSpriteRectAngle({ static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f }, shutdownSideSize, { 0.5f,0.5f }, 45.0f * Math::ToRadian, {}, { 32.0f,32.0f }, shutdownTextureHandle, 0xFFFFFF05);
				TOMATOsEngine::DrawSpriteRectAngle({ static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f }, shutdownSideSize, { 0.5f,0.5f }, 135.0f * Math::ToRadian, {}, { 32.0f,32.0f }, shutdownTextureHandle, 0xFFFFFF05);
			}

			break;
		}

        case inGame:
        {
            operationInstructions.Draw({ 20.0f, 7.0f, 0.0f}, { 1.5f, 1.5f, 0.0f }, OperationInstructions::Type::Move);
            operationInstructions.Draw({ 40.0f, 7.0f, 0.0f}, { 1.5f, 1.5f, 0.0f }, OperationInstructions::Type::Jump);
            operationInstructions.Draw({ 60.0f, 7.0f, 0.0f}, { 1.5f, 1.5f, 0.0f }, OperationInstructions::Type::RedBomb);
            operationInstructions.Draw({ 80.0f, 7.0f, 0.0f}, { 1.5f, 1.5f, 0.0f }, OperationInstructions::Type::WallKick);
            operationInstructions.Draw({100.0f, 7.0f, 0.0f}, { 1.5f, 1.5f, 0.0f }, OperationInstructions::Type::BlueBomb);
            operationInstructions.Draw({120.0f, 7.0f, 0.0f}, { 1.5f, 1.5f, 0.0f }, OperationInstructions::Type::HipDrop);
            operationInstructions.Draw({150.0f, 7.0f, 0.0f}, { 1.5f, 1.5f, 0.0f }, OperationInstructions::Type::Burst);
            backGround.Draw();
            score->Draw();
            stageObjectManager->Draw();

			//backGround.Draw();
			ground->Draw();
			wall->Draw();
			border->Draw();
			player.Draw();
			particleManager->Draw();
			//TOMATOsEngine::DrawSpriteRect({ 0.0f,0.0f }, { static_cast<float>(TOMATOsEngine::kMonitorWidth) ,static_cast<float>(TOMATOsEngine::kMonitorHeight) }, { 0.0f,0.0f }, { 640.0f,480.0f }, floorHandle, 0xFFFFFFFF);
			break;
		}
		case gameClear:
		{
			backGround.ResultDraw();
			player.ResultDraw();
			score->Draw();
			break;
		}
		default:
		{
			break;
		}
		}
	}

	TOMATOsEngine::Shutdown();

	return 0;
}


