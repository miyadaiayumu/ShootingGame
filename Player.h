#pragma once
#include "KamataEngine.h"

class Player {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();
	void Draw();

private:
	KamataEngine::Camera* mCamera = nullptr;

	KamataEngine::WorldTransform mWorldTransform{};

	KamataEngine::Model* mModel = nullptr;

	// 移動速度
	KamataEngine::Vector3 velocity_{};

	// プレイヤーの移動速度
	float moveSpeed_ = 0.2f;
};