#include "Drop.h"
#include "WorldMatrixUtil.h"
#include <assert.h>
#include <cmath>

using namespace KamataEngine;

void Drop::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = {position.x, baseY_, position.z};
	worldTransform_.scale_ = {0.35f, 0.35f, 0.35f};
	worldTransform_.rotation_ = {0.6f, 0.0f, 0.6f}; // 傾けて宝石っぽく
	UpdateWorldMatrix(worldTransform_);
}

void Drop::Update(const Vector3& playerPos, float magnetRange) {
	++timer_;

	// その場でくるくる回りながら上下にふわふわ
	worldTransform_.rotation_.y += 0.08f;

	float dx = playerPos.x - worldTransform_.translation_.x;
	float dz = playerPos.z - worldTransform_.translation_.z;
	float dist = std::sqrt(dx * dx + dz * dz);

	if (dist < kPickupRadius) {
		isCollected_ = true;
		return;
	}

	if (dist < magnetRange) {
		// プレイヤーへ吸い寄せられる
		worldTransform_.translation_.x += dx / dist * kMagnetSpeed;
		worldTransform_.translation_.z += dz / dist * kMagnetSpeed;
	}

	worldTransform_.translation_.y = baseY_ + std::sin(timer_ * 0.1f) * 0.2f;
	UpdateWorldMatrix(worldTransform_);
}

void Drop::Draw() { model_->Draw(worldTransform_, *camera_); }
