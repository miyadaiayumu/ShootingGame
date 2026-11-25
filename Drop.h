#pragma once
#include "KamataEngine.h"

// 敵を倒すと落ちる経験値アイテム（近づくと吸い寄せられて回収される）
class Drop {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update(const KamataEngine::Vector3& playerPos, float magnetRange);
	void Draw();

	bool IsCollected() const { return isCollected_; }
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::WorldTransform worldTransform_{};

	float baseY_ = 0.8f;   // 浮遊の基準の高さ
	int timer_ = 0;        // 浮遊・回転用
	bool isCollected_ = false;

	static inline const float kPickupRadius = 1.2f; // この距離まで近づくと回収
	static inline const float kMagnetSpeed = 0.4f;  // 吸い寄せられる速さ
};
