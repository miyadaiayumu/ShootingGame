#pragma once
#include "Bullet.h"
#include "CameraController.h"
#include "DeathEffect.h"
#include "Drop.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "LevelUpMenu.h"
#include "MapChipField.h"
#include "Player.h"
#include <algorithm>
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

	// クリア演出が終わり、クリア画面へ移ってよいか
	bool IsClearFinished() const { return isCleared_ && clearTimer_ >= kClearDelay; }

	// ライフが尽き、ゲームオーバー画面へ移ってよいか
	bool IsGameOverFinished() const { return isGameOver_ && gameOverTimer_ >= kGameOverDelay; }

private:
	// ===== 敵・弾 =====
	void UpdateWaves();
	void SpawnEnemy(float baseAngle, float spread);
	void UpdateAutoShot();
	void CheckBulletEnemyCollision();

	// ===== ドロップ・レベルアップ =====
	void SpawnDrop(const KamataEngine::Vector3& position);
	void UpdateDrops();
	void CheckLevelUp();
	void ApplyStat(StatType type);
	int GetExpToNext() const { return 3 + (level_ - 1) * 2; }

	// ===== 全体 =====
	void UpdateGameplay(); // ゲーム中の更新
	void Restart();        // 最初からやり直す

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
	KamataEngine::Model* mWallModel = nullptr;
	static inline const float kWallHeightScale = 1.2f; // 壁の高さの倍率（モデルの高さ2 × この値）

	// 敵
	KamataEngine::Model* mEnemyModel = nullptr;
	std::list<Enemy*> enemies_;

	// ===== ウェーブ（画面上の表示はなし） =====
	int wave_ = 0;                // 何ウェーブ目か（0 = まだ始まっていない）
	int waveEnemiesLeft_ = 0;     // このウェーブでまだ出していない敵の数
	int waveSpawnTimer_ = 0;      // 敵を1体ずつ出す間隔のカウント
	int waveRestTimer_ = 0;       // 次のウェーブまでのカウント
	float waveAngle_ = 0.0f;      // このウェーブの敵が来る方向（プレイヤーから見た角度）

	static inline const int kFirstWaveDelay = 120;   // 最初のウェーブまでの時間（フレーム）
	static inline const int kWaveInterval = 360;     // ウェーブとウェーブの間隔（フレーム）
	static inline const int kWaveSpawnDelay = 5;     // ウェーブ中、1体ごとの出現間隔（フレーム）
	static inline const float kWaveSpread = 0.6f;    // 1つのウェーブの敵が広がる角度（ラジアン）
	static inline const int kWaveBaseCount = 6;      // 最初のウェーブの敵の数
	static inline const int kWaveCountStep = 3;      // ウェーブごとに増える数

	static inline const int kWaveMaxCount = 30;      // 1ウェーブの敵の数の上限

	int GetWaveEnemyCount() const { return (std::min)(kWaveBaseCount + (wave_ - 1) * kWaveCountStep, kWaveMaxCount); }

	static inline const float kSpawnMinDist = 20.0f;  // プレイヤーからの出現距離（最小）
	static inline const float kSpawnMaxDist = 30.0f;  // 同（最大）
	static inline const float kSpawnMargin = 0.1f;    // 出現判定の余白（NDC）
	static inline const int kSpawnTryCount = 20;      // 画面外の位置を探す試行回数

	// 弾
	KamataEngine::Model* mBulletModel = nullptr;
	std::list<Bullet*> bullets_;
	int shotTimer_ = 0;

	// ===== 強化できるステータス =====
	static inline const int kBaseShotInterval = 20;   // 発射間隔の初期値（フレーム）
	static inline const float kBaseShotRange = 15.0f; // 射程の初期値
	static inline const int kMinShotInterval = 6;     // 発射間隔の下限
	static inline const float kRangeUpAmount = 3.0f;  // 射程カード1枚あたりの増加量
	static inline const float kMoveSpeedUpAmount = 0.03f;

	int shotInterval_ = kBaseShotInterval;
	float shotRange_ = kBaseShotRange;

	// 各強化を何回選んだか／何回まで選べるか
	int statLevels_[static_cast<int>(StatType::kCount)] = {};
	static inline const int kStatMaxLevels[static_cast<int>(StatType::kCount)] = {5, 5, 5, 3};

	// ===== エフェクト（それぞれ専用のモデルと画像を使う） =====
	KamataEngine::Model* mParticleModel = nullptr;    // 敵を倒したときの破片（赤〜オレンジ）
	DeathEffect* deathEffect_ = nullptr;
	KamataEngine::Model* mPlayerDeathModel = nullptr; // プレイヤーが倒れたときの破片（白〜水色）
	DeathEffect* playerDeathEffect_ = nullptr;
	KamataEngine::Model* mHitModel = nullptr;         // 弾が壁に当たったときの火花（黄色）
	DeathEffect* hitEffect_ = nullptr;
	KamataEngine::Model* mTrailModel = nullptr;       // 弾の軌跡（淡い黄〜橙）
	DeathEffect* trailEffect_ = nullptr;
	int trailTimer_ = 0;

	void UpdateEffects();

	// ===== ドロップ・経験値 =====
	KamataEngine::Model* mDropModel = nullptr;
	std::list<Drop*> drops_;
	int level_ = 1;
	int exp_ = 0;
	static inline const float kMagnetRange = 6.0f; // ドロップが吸い寄せられる距離

	LevelUpMenu* levelUpMenu_ = nullptr;
	bool isLevelUp_ = false;

	// ===== 表示・状態 =====
	static inline const float kDrawMargin = 0.15f; // 描画判定の余白（NDC）

	int debugPrintTimer_ = 0;

	// UI
	KamataEngine::Sprite* heartSprites_[Player::kMaxHpLimit] = {};      // 残っているライフ
	KamataEngine::Sprite* heartEmptySprites_[Player::kMaxHpLimit] = {}; // 失ったライフ（枠）
	static inline const float kHeartSize = 64.0f;
	static inline const float kHeartSpacing = 72.0f;

	// 生き残り時間ゲージ
	KamataEngine::Sprite* timeLabelSprite_ = nullptr;
	KamataEngine::Sprite* timeGaugeBackSprite_ = nullptr;
	KamataEngine::Sprite* timeGaugeFillSprite_ = nullptr;
	static inline const float kGaugeWidth = 400.0f;
	static inline const float kGaugeHeight = 24.0f;

	// 経験値ゲージ（画面下）
	KamataEngine::Sprite* expLabelSprite_ = nullptr;
	KamataEngine::Sprite* expGaugeBackSprite_ = nullptr;
	KamataEngine::Sprite* expGaugeFillSprite_ = nullptr;
	static inline const float kExpGaugeWidth = 600.0f;
	static inline const float kExpGaugeHeight = 16.0f;

	// 状態
	bool isGameOver_ = false;
	bool isCleared_ = false;
	int clearTimer_ = 0;
	int gameOverTimer_ = 0;
	static inline const int kGameOverDelay = 90; // ゲームオーバー後、画面が切り替わるまでの時間（フレーム）
	static inline const int kClearDelay = 60; // クリア後、画面が切り替わるまでの時間（フレーム）

	// 生き残り時間（この時間を耐えきるとクリア）
	int survivalTimer_ = 0;
	static inline const int kClearTimeSeconds = 120;                     // クリアまでの時間（秒）
	static inline const int kClearTimeFrames = kClearTimeSeconds * 60;   // 同（フレーム、60FPS前提）

	// 倒した数（クリア条件ではないが記録しておく）
	int killCount_ = 0;
};
