#pragma once
#include "KamataEngine.h"

// プレイヤーに向かってくるだけのシンプルな敵
class Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update(const KamataEngine::Vector3& targetPos);
	void Draw();

	bool IsDead() const { return isDead_; }
	void Kill() { isDead_ = true; }
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::WorldTransform worldTransform_{};

	float moveSpeed_ = 0.08f;
	bool isDead_ = false;
};
