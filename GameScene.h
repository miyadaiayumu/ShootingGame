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

	
	// BGM/SE用メンバ変数
	uint32_t bgmHandle_ = 0;
	uint32_t voiceHandle_ = 0;

	// SE用メンバ変数
	uint32_t soundEnemyDeadHandle_ = 0; // 敵撃破SE
	uint32_t soundGuardHandle_ = 0;     // ガードSE
	uint32_t soundPlayerDeadHandle_ = 0;


	// カメラ
	KamataEngine::Camera mMainCamera{};

	KamataEngine::DebugCamera* mDebugCamera = nullptr;
	bool isDebugCameraActive_ = false;

	// プレイヤー
	Player* mPlayer = nullptr;
	KamataEngine::Model* mPlayerModel = nullptr;
	KamataEngine::Model* mPlayerAttackModel = nullptr;

	

	// 追従カメラ
	CameraController* cameraController_ = nullptr;

};