#include "WorldMatrixUtil.h"
#include "Enemy.h"
#include <assert.h>
#include <cmath>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	UpdateWorldMatrix(worldTransform_);
}

void Enemy::Update(const Vector3& targetPos) {
	// プレイヤーへの方向（XZ平面のみ）
	float dx = targetPos.x - worldTransform_.translation_.x;
	float dz = targetPos.z - worldTransform_.translation_.z;
	float len = std::sqrt(dx * dx + dz * dz);

	if (len > 0.001f) {
		dx /= len;
		dz /= len;
		worldTransform_.translation_.x += dx * moveSpeed_;
		worldTransform_.translation_.z += dz * moveSpeed_;
		// 進行方向を向く（モデルの正面が+Zの場合。ズレるならオフセットを足す）
		worldTransform_.rotation_.y = std::atan2(dx, dz);
	}

	UpdateWorldMatrix(worldTransform_);
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }
