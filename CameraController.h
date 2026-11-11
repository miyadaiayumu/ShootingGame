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

	// シェイク反映済みのカメラを取得
	const KamataEngine::Camera& GetCamera() const { return camera_; }


private:
	KamataEngine::Camera camera_{};
	const Player* target_ = nullptr;

	KamataEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f};
	Rect movableArea_ = {0.0f, 100.0f, 0.0f, 100.0f};
	KamataEngine::Vector3 targetPosition_{};

	static inline const float kInterpolationRate = 0.1f;
	static inline const float kVelocityBias = 20.0f;
	static inline const Rect kMargin = {-4.0f, 4.0f, -3.0f, 3.0f};

};