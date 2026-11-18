#pragma once
#include "Bullet.h"
#include "CameraController.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
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
	// 敵の出現
	void SpawnEnemy();
	// 自動照準で弾を撃つ
	void UpdateAutoShot();
	// 弾と敵の当たり判定
	void CheckBulletEnemyCollision();
	// ゲーム中の更新（ゲームオーバー中は呼ばない）
	void UpdateGameplay();
	// 最初からやり直す
	void Restart();

	// カメラ
	KamataEngine::Camera mMainCamera{};

	KamataEngine::DebugCamera* mDebugCamera = nullptr;
	bool isDebugCameraActive_ = false;

	// プレイヤー
	Player* mPlayer = nullptr;
	KamataEngine::Model* mPlayerModel = nullptr;

	// 追従カメラ
	CameraController* cameraController_ = nullptr;

	// 床
	MapChipField* mMapChipField = nullptr;
	KamataEngine::Model* mFloorModel = nullptr;

	// 敵
	KamataEngine::Model* mEnemyModel = nullptr;
	std::list<Enemy*> enemies_;
	int enemySpawnTimer_ = 0;

	// 弾
	KamataEngine::Model* mBulletModel = nullptr;
	std::list<Bullet*> bullets_;
	int shotTimer_ = 0;

	int debugPrintTimer_ = 0;

	// ===== UI =====
	KamataEngine::Sprite* heartSprites_[Player::kMaxHp] = {};      // 残っているライフ
	KamataEngine::Sprite* heartEmptySprites_[Player::kMaxHp] = {}; // 失ったライフ（枠）
	KamataEngine::Sprite* gameOverSprite_ = nullptr;
	bool isGameOver_ = false;

	static inline const float kHeartSize = 64.0f;
	static inline const float kHeartSpacing = 72.0f;

	static inline const int kShotInterval = 20;      // 発射間隔（フレーム）
	static inline const float kShotRange = 25.0f;    // 射程（この範囲の敵だけ狙う）

	static inline const float kDrawMargin = 0.15f;   // 描画判定の余白（NDC）
	static inline const float kSpawnMargin = 0.1f;   // 出現判定の余白（NDC）。画面よりこれだけ外側から出す
	static inline const int kSpawnTryCount = 20;     // 画面外の位置を探す試行回数
};
