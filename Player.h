#pragma once
#include "KamataEngine.h"

class MapChipField;

class Player {
public:
	static constexpr int kMaxHp = 3;               // 最大ライフ
	static constexpr int kInvincibleFrames = 90;   // 被ダメージ後の無敵時間（フレーム）

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();
	void Draw();

	// リスタート用：ライフと位置を初期状態に戻す
	void Reset(const KamataEngine::Vector3& position);

	// ダメージを受ける。無敵中は無視して false を返す
	bool TakeDamage(int damage = 1);

	int GetHp() const { return hp_; }
	int GetMaxHp() const { return kMaxHp; }
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
	float moveSpeed_ = 0.2f;

	// ライフ
	int hp_ = kMaxHp;
	// 無敵の残りフレーム
	int invincibleTimer_ = 0;
};
