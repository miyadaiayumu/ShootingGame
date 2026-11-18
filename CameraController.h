#pragma once
#include "KamataEngine.h"

class Player;

class CameraController {
public:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	~CameraController();

	void Initialize();
	void Update();
	void Reset();

	void SetTarget(const Player* target) { target_ = target; }
	void SetMovableArea(const Rect& area) { movableArea_ = area; }

	const KamataEngine::Camera& GetCamera() const { return camera_; }

private:
	void ApplyToCamera();

	KamataEngine::Vector3 ClampToArea(const KamataEngine::Vector3& pos) const;

	KamataEngine::Camera camera_{};
	const Player* target_ = nullptr;

	KamataEngine::Vector3 focusPosition_{};

	KamataEngine::Vector3 cameraOffset_ = {0.0f, 20.0f, -30.0f};

	Rect movableArea_ = {-100.0f, 100.0f, -100.0f, 100.0f};

	// 追従の速さ（0〜1。大きいほど素早く追いつく）
	static inline const float kInterpolationRate = 0.1f;
};