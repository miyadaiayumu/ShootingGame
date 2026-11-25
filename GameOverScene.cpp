#include "GameOverScene.h"

using namespace KamataEngine;

GameOverScene::~GameOverScene() {
	delete backgroundSprite_;
	backgroundSprite_ = nullptr;
	delete gameOverSprite_;
	gameOverSprite_ = nullptr;
	delete restartButton_;
	restartButton_ = nullptr;
	delete titleButton_;
	titleButton_ = nullptr;
}

void GameOverScene::Initialize() {
	result_ = Result::kNone;

	const float winW = static_cast<float>(WinApp::kWindowWidth);
	const float winH = static_cast<float>(WinApp::kWindowHeight);

	// 暗い背景
	uint32_t whiteTex = TextureManager::Load("ui/white.png");
	backgroundSprite_ = Sprite::Create(whiteTex, {0.0f, 0.0f}, {0.08f, 0.02f, 0.04f, 1.0f});
	backgroundSprite_->SetSize({winW, winH});

	// 「GAME OVER」（800x170）
	uint32_t gameOverTex = TextureManager::Load("ui/gameover_title.png");
	gameOverSprite_ = Sprite::Create(gameOverTex, {(winW - 800.0f) * 0.5f, 140.0f});

	// ボタン2つ（320x84）を横に並べる
	const float gap = 40.0f;
	const float y = 420.0f;
	uint32_t restartTex = TextureManager::Load("ui/btn_restart.png");
	uint32_t titleTex = TextureManager::Load("ui/btn_title.png");

	restartButton_ = new Button();
	restartButton_->Initialize(restartTex, {winW * 0.5f - 320.0f - gap * 0.5f, y}, {320.0f, 84.0f});

	titleButton_ = new Button();
	titleButton_->Initialize(titleTex, {winW * 0.5f + gap * 0.5f, y}, {320.0f, 84.0f});
}

void GameOverScene::Update() {
	restartButton_->Update();
	titleButton_->Update();

	if (restartButton_->IsClicked()) {
		result_ = Result::kRestart;
	} else if (titleButton_->IsClicked()) {
		result_ = Result::kTitle;
	}
}

void GameOverScene::Draw() {
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	backgroundSprite_->Draw();
	gameOverSprite_->Draw();
	restartButton_->Draw();
	titleButton_->Draw();

	Sprite::PostDraw();
}
