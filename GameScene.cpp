#include "GameScene.h"
#include <imgui.h>

using namespace KamataEngine;

GameScene::~GameScene() {

	delete mPlayer;
	mPlayer = nullptr;

	delete mPlayerModel;
	mPlayerModel = nullptr;


	delete mDebugCamera;
	mDebugCamera = nullptr;

	delete cameraController_;
	cameraController_ = nullptr;
}

void GameScene::Initialize() {
	// カメラ初期化
	mMainCamera.Initialize();

	// farZを伸ばす
	mMainCamera.farZ = 1000.0f;

	// 行列更新
	mMainCamera.UpdateMatrix();

	mDebugCamera = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// プレイヤーモデル読み込み
	mPlayerModel = Model::CreateFromOBJ("Player");

	// プレイヤー生成
	mPlayer = new Player();

	// プレイヤー初期化
	mPlayer->Initialize(mPlayerModel, &mMainCamera, {0.0f, 0.0f, 0.0f});


	// ===== カメラコントロール =====
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(mPlayer);

	CameraController::Rect cameraArea = {0.0f, 100.0f, 0.0f, 100.0f};
	cameraController_->SetMovableArea(cameraArea);
	cameraController_->Reset();
}

void GameScene::Update() {

	
		mPlayer->Update();



	// --- カメラの更新処理 ---
	bool isDebugMode = false;

#ifdef _DEBUG
	Input* input = Input::GetInstance();

	if (input->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	isDebugMode = isDebugCameraActive_;
#endif

	if (isDebugMode && mDebugCamera) {
		mDebugCamera->Update();
		mMainCamera.matView = mDebugCamera->GetCamera().matView;
		mMainCamera.matProjection = mDebugCamera->GetCamera().matProjection;
		mMainCamera.TransferMatrix();
	} else {
		mMainCamera.matView = cameraController_->GetCamera().matView;
		mMainCamera.matProjection = cameraController_->GetCamera().matProjection;
		mMainCamera.translation_ = cameraController_->GetCamera().translation_;
		mMainCamera.rotation_ = cameraController_->GetCamera().rotation_;
		mMainCamera.TransferMatrix();
	}

	
}

void GameScene::Draw() {
	Model::PreDraw();

		// プレイヤー
		mPlayer->Draw();

		Model::PostDraw();
	
}
