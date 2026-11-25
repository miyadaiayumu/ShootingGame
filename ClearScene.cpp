#include "ClearScene.h"

using namespace KamataEngine;

ClearScene::~ClearScene() {
	delete backgroundSprite_;
	backgroundSprite_ = nullptr;
	delete clearSprite_;
	clearSprite_ = nullptr;
	delete restartButton_;
	restartButton_ = nullptr;
	delete titleButton_;
	titleButton_ = nullptr;
}

void ClearScene::Initialize() {
	result_ = Result::kNone;

	const float winW = static_cast<float>(WinApp::kWindowWidth);
	const float winH = static_cast<float>(WinApp::kWindowHeight);

	// 暗い背景
	uint32_t whiteTex = TextureManager::Load("ui/white.png");
	backgroundSprite_ = Sprite::Create(whiteTex, {0.0f, 0.0f}, {0.04f, 0.06f, 0.12f, 1.0f});
	backgroundSprite_->SetSize({winW, winH});

	// 「STAGE CLEAR」（800x170）
	uint32_t clearTex = TextureManager::Load("ui/clear_title.png");
	clearSprite_ = Sprite::Create(clearTex, {(winW - 800.0f) * 0.5f, 140.0f});

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

void ClearScene::Update() {
	restartButton_->Update();
	titleButton_->Update();

	if (restartButton_->IsClicked()) {
		result_ = Result::kRestart;
	} else if (titleButton_->IsClicked()) {
		result_ = Result::kTitle;
	}
}

void ClearScene::Draw() {
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	backgroundSprite_->Draw();
	clearSprite_->Draw();
	restartButton_->Draw();
	titleButton_->Draw();

	Sprite::PostDraw();
}
