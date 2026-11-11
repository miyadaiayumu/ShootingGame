#include "GameScene.h"
#include <imgui.h>

using namespace KamataEngine;

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 matrix{};

	float cosX = cosf(rotate.x);
	float sinX = sinf(rotate.x);
	float cosY = cosf(rotate.y);
	float sinY = sinf(rotate.y);
	float cosZ = cosf(rotate.z);
	float sinZ = sinf(rotate.z);

	matrix.m[0][0] = scale.x * (cosY * cosZ);
	matrix.m[0][1] = scale.x * (cosY * sinZ);
	matrix.m[0][2] = scale.x * (-sinY);
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = scale.y * (sinX * sinY * cosZ - cosX * sinZ);
	matrix.m[1][1] = scale.y * (sinX * sinY * sinZ + cosX * cosZ);
	matrix.m[1][2] = scale.y * (sinX * cosY);
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = scale.z * (cosX * sinY * cosZ + sinX * sinZ);
	matrix.m[2][1] = scale.z * (cosX * sinY * sinZ - sinX * cosZ);
	matrix.m[2][2] = scale.z * (cosX * cosY);
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = translate.x;
	matrix.m[3][1] = translate.y;
	matrix.m[3][2] = translate.z;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

GameScene::~GameScene() {
	// ===== BGMの停止 =====
	Audio::GetInstance()->StopWave(voiceHandle_);

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


	// ===== BGMの読み込みと再生 =====
	bgmHandle_ = Audio::GetInstance()->LoadWave("music/play.mp3");
	voiceHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
	Audio::GetInstance()->SetVolume(voiceHandle_, 0.5f);

	// ===== SEの読み込み =====
	soundEnemyDeadHandle_ = Audio::GetInstance()->LoadWave("sound/enemyDead.mp3");
	soundGuardHandle_ = Audio::GetInstance()->LoadWave("sound/guard.mp3");
	soundPlayerDeadHandle_ = Audio::GetInstance()->LoadWave("sound/death.mp3");


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


		uint32_t handle = Audio::GetInstance()->PlayWave(soundPlayerDeadHandle_, false);
		Audio::GetInstance()->SetVolume(handle, 0.7f);


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
