#define NOMINMAX
#include "Player.h"
#include <algorithm>
#include <array>
#include <assert.h>
#include <cmath>
#include <numbers>

using namespace KamataEngine;


void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	assert(camera);

	mCamera = camera;
	mModel = model;
	
	mWorldTransform.Initialize();
	mWorldTransform.translation_ = position;

	
	mWorldTransform.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	
}

void Player::Update() {

	// 入力取得
	Input* input = Input::GetInstance();

	// 移動方向を初期化
	velocity_ = {0.0f, 0.0f, 0.0f};

	// 上下移動（Z軸）
	if (input->PushKey(DIK_W)) {
		velocity_.z -= 1.0f;
	}

	if (input->PushKey(DIK_S)) {
		velocity_.z += 1.0f;
	}

	// 左右移動（X軸）
	if (input->PushKey(DIK_A)) {
		velocity_.x -= 1.0f;
	}

	if (input->PushKey(DIK_D)) {
		velocity_.x += 1.0f;
	}

	// 斜め移動の速度調整
	float length = std::sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);

	if (length > 0.0f) {
		velocity_.x /= length;
		velocity_.z /= length;
	}

	// 座標に移動量を反映
	mWorldTransform.translation_.x += velocity_.x * moveSpeed_;
	mWorldTransform.translation_.z += velocity_.z * moveSpeed_;

	// ワールド行列更新
	mWorldTransform.TransferMatrix();
}

// ===== 描画処理 =====
void Player::Draw() {
	mModel->Draw(mWorldTransform, *mCamera);

}