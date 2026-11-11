#include "CameraController.h"
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

void CameraController::Initialize() {
	camera_.Initialize();

	// カメラの初期位置
	camera_.translation_ = {0.0f, 20.0f, -30.0f};

	// カメラの角度
	camera_.rotation_ = {0.8f, 0.0f, 0.0f};

	camera_.UpdateMatrix();
}

void CameraController::Update() {

	// 本来の移動位置を一時保存
	Vector3 baseTranslation = camera_.translation_;

	// ビュー行列の更新
	camera_.UpdateMatrix();

	// 本来の位置に戻す
	camera_.translation_ = baseTranslation;
}

void CameraController::Reset() {

	// プレイヤーが存在しない場合
	if (!target_) {
		return;
	}

	// カメラの位置
	camera_.translation_ = {0.0f, 20.0f, -30.0f};

	// カメラの角度
	camera_.rotation_ = {0.8f, 0.0f, 0.0f};

	camera_.UpdateMatrix();
}

CameraController::~CameraController() {}