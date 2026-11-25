#pragma once
#include "KamataEngine.h"

class MapChipField;

// プレイヤーに向かってくる敵（穴を避けて進む）
class Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// field があれば、穴を避けて targetPos へ向かう
	void Update(const KamataEngine::Vector3& targetPos, const MapChipField* field);
	void Draw();

	// 倒された（当たり判定・狙われる対象から外れる）。やられ演出が終わるまでは画面に残る
	bool IsDead() const { return isDead_; }
	// やられ演出が終わり、リストから消してよい
	bool IsRemovable() const { return isDead_ && deathTimer_ >= kDeathFrames; }

	void Kill() {
		if (!isDead_) {
			isDead_ = true;
			deathTimer_ = 0;
		}
	}
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::WorldTransform worldTransform_{};

	// やられ演出（ぷくっと膨らんでから、回りながら縮んで消える）
	void UpdateDeath();

	float moveSpeed_ = 0.08f;
	bool isDead_ = false;
	int deathTimer_ = 0;

	static inline const int kDeathFrames = 10; // やられ演出の長さ（フレーム）

	// ===== モデル（enemy.obj）に合わせた設定 =====
	static inline const float kModelScale = 1.6f;                // モデルの大きさの倍率
	static inline const float kModelBottomY = 0.394f;            // モデルの足元の位置（原点から下へ）
	static inline const float kGroundOffsetY = kModelBottomY * kModelScale; // 足元が床(y=0)に付く高さ
	static inline const float kModelYawOffset = 0.0f;            // 向きがずれて見えるときに調整（ラジアン）
};
