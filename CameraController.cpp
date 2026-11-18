#include "CameraController.h"
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

CameraController::~CameraController() {}

void CameraController::Initialize() {
	camera_.Initialize();

	// カメラの角度（見下ろし）
	camera_.rotation_ = {0.8f, 0.0f, 0.0f};

	ApplyToCamera();
}

void CameraController::Reset() {
	if (!target_) {
		return;
	}

	// 補間なしで一気にプレイヤーの位置へ
	focusPosition_ = ClampToArea(target_->GetWorldPosition());
	ApplyToCamera();
}

void CameraController::Update() {
	if (target_) {
		Vector3 goal = ClampToArea(target_->GetWorldPosition());

		// 目標へ少しずつ近づく
		focusPosition_.x += (goal.x - focusPosition_.x) * kInterpolationRate;
		focusPosition_.z += (goal.z - focusPosition_.z) * kInterpolationRate;
	}

	ApplyToCamera();
}

void CameraController::ApplyToCamera() {
	camera_.translation_ = {
	    focusPosition_.x + cameraOffset_.x,
	    cameraOffset_.y,
	    focusPosition_.z + cameraOffset_.z,
	};
	camera_.UpdateMatrix();
}

Vector3 CameraController::ClampToArea(const Vector3& pos) const {
	Vector3 result = pos;
	result.x = std::clamp(result.x, movableArea_.left, movableArea_.right);
	result.z = std::clamp(result.z, movableArea_.bottom, movableArea_.top);
	return result;
}