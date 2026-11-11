#pragma once
#include "CameraController.h"
#include "KamataEngine.h"
#include "Player.h"
#include <list>
#include <vector>

class GameScene {
public:


	GameScene() = default;
	~GameScene();

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	

private:
	void ChangePhase();



	// カメラ
	KamataEngine::Camera mMainCamera{};

	KamataEngine::DebugCamera* mDebugCamera = nullptr;
	bool isDebugCameraActive_ = false;

	// プレイヤー
	Player* mPlayer = nullptr;
	KamataEngine::Model* mPlayerModel = nullptr;


	// 追従カメラ
	CameraController* cameraController_ = nullptr;

};