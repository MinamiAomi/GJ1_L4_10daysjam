#include "ParticleManager.h"

#include "TOMATOsEngine.h"

ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Initialize() {
	// 円
	circle_ = new Circle();
	circle_->Initialize();
	// ついてく
	follow_ = new Follow();
	follow_->Initialize();
	// スプラッシュ
	splash_ = new Splash();
	splash_->Initialize();
	// ポップ
	pop_ = new Pop();
	pop_->Initialize();
	// エミッターの回りを回る
	yenLetter_ = new YenLetter();
	yenLetter_->Initialize();
	// 縦のライン
	verticleLine_ = new VerticalLine();
	verticleLine_->Initialize();
	//Number
	number_ = new Number();
	number_->Initialize();
}

void ParticleManager::Update() {
#ifdef _DEBUG
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("ParticleManager")) {
		if (ImGui::Button("Create")) {
			//circle_->Create({20.0,20.0f},Vector4(0.0f,0.0f,1.0f,1.0f),0,5);
			//follow_->Create({25.0,20.0f},Vector4(0.0f,1.0f,0.0f,1.0f),0,5);
			//splash_->Create({ 40.0,20.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, 20);
			number_->Create({ 100.0,20.0f }, { 1.0f, 1.0f,1.0f,1.0f },10);
			//pop_->Create({ 40.0,20.0f }, Vector4(1.0f, 1.0f, 1.0f, 1.0f), 10);
			//pop_->Create({40.0,20.0f},Vector4(1.0f,1.0f,1.0f,1.0f),10);
			//yenLetter_->Create({40.0,20.0f},Vector4(1.0f,1.0f,0.0f,1.0f),0);
			//verticleLine_->Create({ 45.0,20.0f }, Vector4(1.0f, 0.0f, 1.0f, 1.0f), 0, {5.0f,1.0f});
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

	circle_->Update();
	follow_->Update();
	splash_->Update();
	pop_->Update();
	yenLetter_->Update();
	verticleLine_->Update();
	number_->Update();
}

void ParticleManager::Draw() {
	TOMATOsEngine::SetBlendMode(kBlendModeNormal);
	follow_->Draw();
	TOMATOsEngine::SetBlendMode(kBlendModeAdditive);
	circle_->Draw();
	splash_->Draw();
	pop_->Draw();
	yenLetter_->Draw();
	verticleLine_->Draw();
	number_->Draw();
	TOMATOsEngine::SetBlendMode(kBlendModeNormal);
}
