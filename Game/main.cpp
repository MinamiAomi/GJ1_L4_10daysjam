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

#include "BackGround.h"
#include "Player.h"
#include "Ground.h"
#include "Border.h"
#include "Score.h"
#include "Wall.h"
#include "Title.h"
#include "HexagonSevenSegmentDisplay.h"

#define INVALID_PLAY_HANDLE (size_t(-1))
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {


    TOMATOsEngine::Initialize();
#ifdef _DEBUG
    TOMATOsEngine::SetFullScreen(false);
#endif // _DEBUG

    enum GameScene {
        title,
        inGame,
        gameClear,
    };

    GameScene gameScene = title;

    Vector3 cameraPosition = { 0.0f, 3.0f, -50.0f };
    Vector3 cameraRotate = {};
    Camera camera;
    camera.SetPosition({ 0.0f, 3.0f, -10.0f });



#pragma region テクスチャハンドル
    //タイトル
    TextureHandle titleHandle = TOMATOsEngine::LoadTexture("Resources/BBtitle.png");
    TextureHandle startTextureHandle = TOMATOsEngine::LoadTexture("Resources/startText.png");
    TextureHandle operationTextureHandle = TOMATOsEngine::LoadTexture("Resources/operationText.png");
    TextureHandle endTextureHandle = TOMATOsEngine::LoadTexture("Resources/endText.png");
    TextureHandle arrowTextureHandle = TOMATOsEngine::LoadTexture("Resources/arrow.png");
    //SpaceかBボタンを押してね画像
    TextureHandle spaceorBTextureHandle = TOMATOsEngine::LoadTexture("Resources/spaceorb.png");
    //ゲームオーバー
    TextureHandle gameOverHandle = TOMATOsEngine::LoadTexture("Resources/gameOver.png");

    //モニター内の地面
    TextureHandle floorHandle = TOMATOsEngine::LoadTexture("Resources/floor.png");

    //シャットダウン
    TextureHandle shutdownTextureHandle = TOMATOsEngine::LoadTexture("Resources/shpere.png");
#pragma endregion

#pragma region テクスチャサイズ/ポジション
    //SpaceかBボタンを押してね画像のサイズポジション
    Vector2 spaceorBPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f - 130.0f };
    Vector2 spaceorBSize = { 150.0f * 1.5f,32.0 * 1.5f };


    //gameOver画像のサイズポジション
    Vector2 gameOverPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f - 32.0f * 2.0f };
    Vector2 gameOverSize = { 320.0f,240.0f };
#pragma endregion

#pragma region 矢印
    Vector2 arrowTextSize = { 64.0f * 2.5f,32.0f * 2.5f };
    //スタート・操作説明・終了のポジション
    Vector2 startTextPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,static_cast<float>(TOMATOsEngine::kMonitorHeight) * 0.5f - 180.0f };
    Vector2 operationTextPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,startTextPosition.y - arrowTextSize.y * 0.5f - 10.0f };
    Vector2 endTextPosition = { static_cast<float>(TOMATOsEngine::kMonitorWidth) * 0.5f ,operationTextPosition.y - arrowTextSize.y * 0.5f - 10.0f };
    ///スタート->0,操作説明->1,終了->2
    enum class TitleSceneState {
        start,
        operation,
        end,

        count
    } titleSceneState = TitleSceneState::start;


    //タイトル画面の選択用矢印
    Vector2 arrowPosition = { startTextPosition.x - arrowTextSize.x * 0.5f - 60.0f,startTextPosition.y };
    Vector2 arrowSize = { 32.0f,32.0f };
    uint32_t arrowAnimation = 0;
    uint32_t arrowColor = 0xFFFFFFFF;

    //矢印をちかちかさせるため
    bool flag_ = false;
    //操作説明もÐ－ト
    bool isSwitchViewMode = false;
#pragma endregion

#pragma region 音
	//イワシロ音楽素材 使用の際は追記
	auto pushSpaceSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/pushSpace.wav");
	auto titleSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/titleBGM.wav");
	auto ingameSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/ingameBGM.wav");
	auto clearSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/clearBGM.wav");
	size_t pickHandle = TOMATOsEngine::LoadAudio("Resources/Audio/pick.wav");
	auto shutdownSoundHandle = TOMATOsEngine::LoadAudio("Resources/Audio/shutdown.wav");

	// タイトルははじめから流す
	size_t titlePlayHandle = TOMATOsEngine::PlayAudio(titleSoundHandle, true);
	TOMATOsEngine::SetVolume(titlePlayHandle, 0.8f);
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

    Ground* ground = Ground::GetInstance();

    //初期化にWall使用
    Border* border = Border::GetInstance();
    border->Initialize();

    ParticleManager* particleManager = ParticleManager::GetInstance();
    particleManager->Initialize();

    Player player;
    player.Initialize();
    collisionManager->SetPlayer(&player);

	BackGround backGround;
	backGround.Initialize();
	

    Score score;
    score.Initialize();
    
    //変数名かぶり
    Title title_;
    title_.Initialize();
    //Test
    //HexagonSevenSegmentDisplay* hexagonSevenSegmentDisplay = HexagonSevenSegmentDisplay::GetInstance();
    //int hexagonSevenSegmentDisplayNumber = 0;
#pragma endregion

    while (TOMATOsEngine::BeginFrame()) {
        auto pad = TOMATOsEngine::GetGamePadState();
        auto prepad = TOMATOsEngine::GetGamePadPreState();
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

        camera.SetPosition(cameraPosition);
        camera.SetRotate(Quaternion::MakeFromEulerAngle(cameraRotate * Math::ToRadian));
        camera.UpdateMatrices();
        TOMATOsEngine::SetCameraMatrix(camera.GetViewProjectionMatrix());

        ////////////////////////////////////////////////////更新////////////////////////////////////////////////////////
        switch (gameScene) {
        case title:
        {
            title_.Update();

            //シャットダウン
            if (TOMATOsEngine::IsKeyTrigger(DIK_ESCAPE)) {
                isShutdown = true;
                TOMATOsEngine::PlayAudio(shutdownSoundHandle);
            }

            if (!isShutdown) {
                const uint32_t kArrowAnimation = 25;
                if (!isSwitchViewMode) {

                    //矢印のアップデート
                    arrowAnimation++;
                    if (arrowAnimation >= kArrowAnimation) {
                        if (flag_) {
                            arrowColor = 0xFFFFFFFF;
                        }
                        else {
                            arrowColor = 0xFFFFFF00;
                        }
                        flag_ ^= true;
                        arrowAnimation = 0;
                    }
#pragma region キー入力
                    //下押したとき
                    if (TOMATOsEngine::IsKeyTrigger(DIK_S) ||
                        TOMATOsEngine::IsKeyTrigger(DIK_DOWN) ||
                        ((pad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && !(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) ||
                        (pad.Gamepad.sThumbLY < -5000 &&
                            prepad.Gamepad.sThumbLY <= 4000.0f &&
                            prepad.Gamepad.sThumbLY >= -4000.0f)) {
                        //カウントがオーバーしてたら
                        int currentStateInt = static_cast<int>(titleSceneState);
                        int nextStateInt = currentStateInt + 1;

                        //次が範囲外か
                        if (nextStateInt >= static_cast<int>(TitleSceneState::count)) {
                            titleSceneState = TitleSceneState::start;
                        }
                        else {
                            titleSceneState = static_cast<TitleSceneState>(nextStateInt);
                        }
                        //移動音
                        TOMATOsEngine::PlayAudio(pickHandle);
                        //移動した後に矢印がどこにあるのかはっきりさせたいので色をリセット
                        arrowColor = 0xFFFFFFFF;
                    }
                    //上押したとき
                    if (TOMATOsEngine::IsKeyTrigger(DIK_W) ||
                        TOMATOsEngine::IsKeyTrigger(DIK_UP) ||
                        ((pad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && !(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)) ||
                        (pad.Gamepad.sThumbLY > +5000 &&
                            prepad.Gamepad.sThumbLY <= 4000.0f &&
                            prepad.Gamepad.sThumbLY >= -4000.0f)) {

                        //カウントがオーバーしてたら
                        int currentStateInt = static_cast<int>(titleSceneState);
                        int preStateInt = currentStateInt - 1;

                        //次が範囲外か
                        if (preStateInt < 0) {
                            int lastStateInt = static_cast<int>(TitleSceneState::count) - 1;
                            titleSceneState = static_cast<TitleSceneState>(lastStateInt);
                        }
                        else {
                            titleSceneState = static_cast<TitleSceneState>(preStateInt);
                        }
                        //移動音
                        TOMATOsEngine::PlayAudio(pickHandle);
                        //移動した後に矢印がどこにあるのかはっきりさせたいので色をリセット
                        arrowColor = 0xFFFFFFFF;
                    }
                    if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE) ||
                        ((pad.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
                            !(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
                        TOMATOsEngine::PlayAudio(pickHandle);


                        switch (titleSceneState)
                        {
                        case TitleSceneState::start:
                            gameScene = inGame;

                            collisionManager->Initialize();
                            stageObjectManager->Initialize();
                            spawnManager->Initialize();

                            wall->Initialize();
                            border->Initialize();
                            particleManager->Initialize();

                            backGround.Initialize();
                            player.Initialize();
                            player.SetPosition({ 100.0f * 0.5f, 300.0f - 100.0f });
                            score.Initialize();

							// 音
							// タイトルBGM停止
							TOMATOsEngine::StopAudio(titlePlayHandle);
							// インゲームBGM
							ingamePlayHandle = TOMATOsEngine::PlayAudio(ingameSoundHandle, true);
							TOMATOsEngine::SetVolume(ingamePlayHandle, 0.8f);
							break;
						case TitleSceneState::operation:
							TOMATOsEngine::SwitchViewMode();
							isSwitchViewMode = true;
							break;
						case TitleSceneState::end:
							isShutdown = true;
							TOMATOsEngine::PlayAudio(shutdownSoundHandle);
							break;
						case TitleSceneState::count:
							break;
						default:
							break;
						}
					}
#pragma endregion
                    //矢印のPositionを指定
                    switch (titleSceneState)
                    {
                    case TitleSceneState::start:
                        arrowPosition.y = startTextPosition.y;
                        break;
                    case TitleSceneState::operation:
                        arrowPosition.y = operationTextPosition.y;
                        break;
                    case TitleSceneState::end:
                        arrowPosition.y = endTextPosition.y;
                        break;
                    case TitleSceneState::count:
                        break;
                    default:
                        break;
                    }

                }
                //操作説明から抜け出す
                else if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE) ||
                    (pad.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
                    !(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
                    TOMATOsEngine::PlayAudio(pickHandle);
                    TOMATOsEngine::SwitchViewMode();
                    isSwitchViewMode = false;
                }
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
            wall->Update();
            stageObjectManager->Update();
            spawnManager->Update();

            backGround.Update();
            player.Update();

            border->Update();

            collisionManager->Update();

            score.Update();

            particleManager->Update();

            if (TOMATOsEngine::IsKeyTrigger(DIK_ESCAPE)) {
                TOMATOsEngine::RequestQuit();
            }
            // 音
        /*	if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE)) {
                gameScene = gameClear;
                TOMATOsEngine::StopAudio(ingamePlayHandle);
                ingamePlayHandle = INVALID_PLAY_HANDLE;
            }*/
            break;
        }
        case gameClear:
        {
            player.Update();

            if (!clearToTitle) {
                // ゲームクリアBGM
                clearPlayHandle = TOMATOsEngine::PlayAudio(clearSoundHandle, true);
                TOMATOsEngine::SetVolume(clearPlayHandle, 0.8f);
                clearToTitle = true;
            }

            //タイトルに移動
            if (TOMATOsEngine::IsKeyTrigger(DIK_SPACE) ||
                ((pad.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
                    !(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {

                arrowPosition = { startTextPosition.x - arrowTextSize.x * 0.5f - 30.0f,startTextPosition.y };
                titleSceneState = TitleSceneState::start;
                arrowAnimation = 0;

                gameScene = title;

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
                TOMATOsEngine::SetVolume(titlePlayHandle, 0.2f);
                // スペースオン
                auto pushSpacePlayHandle = TOMATOsEngine::PlayAudio(pushSpaceSoundHandle);
                TOMATOsEngine::SetVolume(pushSpacePlayHandle, 0.1f);
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

        ////////////////////////////////////////////////////更新////////////////////////////////////////////////////////
        switch (gameScene) {
        case title:
        {
            title_.Draw();
            //TOMATOsEngine::DrawSpriteRect({ 0.0f,0.0f }, { static_cast<float>(TOMATOsEngine::kMonitorWidth) ,static_cast<float>(TOMATOsEngine::kMonitorHeight) }, { 0.0f,0.0f }, { 640.0f,480.0f }, titleHandle, 0xFFFFFFFF);

            TOMATOsEngine::DrawSpriteRectAngle(spaceorBPosition, spaceorBSize, { 0.5f,0.5f }, 0.0f, {}, { 150.0f, 32.0f }, spaceorBTextureHandle, 0xFFFFFFFF);
            // スタート
            TOMATOsEngine::DrawSpriteRectAngle(startTextPosition, arrowTextSize, { 0.5f,0.5f }, 0.0f, {}, { 64.0f, 32.0f }, startTextureHandle, 0xFFFFFFFF);
            // 操作
            TOMATOsEngine::DrawSpriteRectAngle(operationTextPosition, arrowTextSize, { 0.5f,0.5f }, 0.0f, {}, { 64.0f, 32.0f }, operationTextureHandle, 0xFFFFFFFF);
            // 終わり
            TOMATOsEngine::DrawSpriteRectAngle(endTextPosition, arrowTextSize, { 0.5f,0.5f }, 0.0f, {}, { 64.0f, 32.0f }, endTextureHandle, 0xFFFFFFFF);
            // 矢印
            TOMATOsEngine::DrawSpriteRectAngle(arrowPosition, arrowSize, { 0.5f,0.5f }, 0.0f, {}, { 32.0f, 32.0f }, arrowTextureHandle, arrowColor);
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
			backGround.Draw();
			score.Draw();
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
			backGround.Draw();
			player.Draw();
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