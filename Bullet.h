#pragma once
#include "KamataEngine.h"

// プレイヤーが撃つ弾（まっすぐ飛ぶだけ）
class Bullet {
public:
	// direction はXZ平面の単位ベクトル
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, const KamataEngine::Vector3& direction);
	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }
	void Kill() { isDead_ = true; }
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::WorldTransform worldTransform_{};

	KamataEngine::Vector3 velocity_{};

	static inline const float kScale = 0.7f; // モデルの大きさの倍率
	static inline const float kSpeed = 0.6f;
	static inline const int kLifeTime = 120; // フレーム

	int lifeTimer_ = kLifeTime;
	bool isDead_ = false;
};
