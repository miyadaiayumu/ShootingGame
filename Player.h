#pragma once
#include "KamataEngine.h"

class MapChipField;

class Player {
public:
	static constexpr int kInitialMaxHp = 3;        // 最初の最大ライフ
	static constexpr int kMaxHpLimit = 6;          // 強化で増やせる上限
	static constexpr int kInvincibleFrames = 90;   // 被ダメージ後の無敵時間（フレーム）
	static constexpr float kBaseMoveSpeed = 0.2f;  // 最初の移動速度
	static constexpr float kTurnRate = 0.25f;      // 向きを変える速さ（0〜1、大きいほど素早い）

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();
	void Draw();

	// リスタート用：ライフ・位置・強化を初期状態に戻す
	void Reset(const KamataEngine::Vector3& position);

	// ダメージを受ける。無敵中は無視して false を返す
	bool TakeDamage(int damage = 1);

	// ===== 強化 =====
	void AddMoveSpeed(float amount) { moveSpeed_ += amount; }
	// 最大ライフを1増やして1回復（上限あり）
	void AddMaxHp();

	int GetHp() const { return hp_; }
	int GetMaxHp() const { return maxHp_; }
	bool IsDead() const { return hp_ <= 0; }
	bool IsInvincible() const { return invincibleTimer_ > 0; }

	const KamataEngine::Vector3& GetWorldPosition() const { return mWorldTransform.translation_; }

	void SetMapChipField(const MapChipField* field) { mMapChipField = field; }

private:
	KamataEngine::Camera* mCamera = nullptr;

	KamataEngine::WorldTransform mWorldTransform{};

	KamataEngine::Model* mModel = nullptr;

	const MapChipField* mMapChipField = nullptr;

	// 移動速度
	KamataEngine::Vector3 velocity_{};

	// プレイヤーの移動速度
	float moveSpeed_ = kBaseMoveSpeed;

	// ライフ
	int maxHp_ = kInitialMaxHp;
	int hp_ = kInitialMaxHp;
	// 無敵の残りフレーム
	int invincibleTimer_ = 0;
};
