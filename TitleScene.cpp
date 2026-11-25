#include "TitleScene.h"

using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete backgroundSprite_;
	backgroundSprite_ = nullptr;
	delete titleSprite_;
	titleSprite_ = nullptr;
	delete startButton_;
	startButton_ = nullptr;
}

void TitleScene::Initialize() {
	finished_ = false;

	const float winW = static_cast<float>(WinApp::kWindowWidth);
	const float winH = static_cast<float>(WinApp::kWindowHeight);

	// 暗い背景
	uint32_t whiteTex = TextureManager::Load("ui/white.png");
	backgroundSprite_ = Sprite::Create(whiteTex, {0.0f, 0.0f}, {0.04f, 0.06f, 0.12f, 1.0f});
	backgroundSprite_->SetSize({winW, winH});

	// タイトルロゴ（1100x230）
	uint32_t titleTex = TextureManager::Load("ui/title.png");
	titleSprite_ = Sprite::Create(titleTex, {(winW - 1100.0f) * 0.5f, 110.0f});

	// スタートボタン（320x84）
	uint32_t startTex = TextureManager::Load("ui/btn_start.png");
	startButton_ = new Button();
	startButton_->Initialize(startTex, {(winW - 320.0f) * 0.5f, 430.0f}, {320.0f, 84.0f});
}

void TitleScene::Update() {
	startButton_->Update();

	if (startButton_->IsClicked()) {
		finished_ = true;
	}
}

void TitleScene::Draw() {
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	backgroundSprite_->Draw();
	titleSprite_->Draw();
	startButton_->Draw();

	Sprite::PostDraw();
}
