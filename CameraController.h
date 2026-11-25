#pragma once
#include "KamataEngine.h"

class Player;

// プレイヤーに追従する見下ろしカメラ
class CameraController {
public:
	// 追従する「注視点」が動ける範囲（ワールドXZ）
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
	// 注視点 + オフセットをカメラ位置に反映して行列を更新
	void ApplyToCamera();
	// 注視点を移動可能範囲に収める
	KamataEngine::Vector3 ClampToArea(const KamataEngine::Vector3& pos) const;

	KamataEngine::Camera camera_{};
	const Player* target_ = nullptr;

	// 注視点（プレイヤーを滑らかに追う）
	KamataEngine::Vector3 focusPosition_{};

	// 注視点からのカメラ位置のずれ
	KamataEngine::Vector3 cameraOffset_ = {0.0f, 17.0f, -16.5f};

	Rect movableArea_ = {-100.0f, 100.0f, -100.0f, 100.0f};

	// 追従の速さ（0〜1。大きいほど素早く追いつく）
	static inline const float kInterpolationRate = 0.1f;
};
