#include "CameraController.h"
#include "Player.h"
#include <algorithm>


void CameraController::Initialize() {
	camera_.Initialize();

}

void CameraController::Update() {
	

	// 本来の移動位置を一時保存
	KamataEngine::Vector3 baseTranslation = camera_.translation_;

	// ビュー行列の更新（これで camera_.matView にシェイク位置が計算される）
	camera_.UpdateMatrix();

	// 次フレームの追従計算が破綻しないよう本来の基本位置に戻す
	camera_.translation_ = baseTranslation;
}

void CameraController::Reset() {
	if (!target_)
		return;


	camera_.translation_ = targetPosition_;
	camera_.UpdateMatrix();
}

CameraController::~CameraController() {}