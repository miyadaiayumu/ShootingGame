#include "Bullet.h"
#include "WorldMatrixUtil.h"
#include <assert.h>
#include <cmath>

using namespace KamataEngine;

void Bullet::Initialize(Model* model, Camera* camera, const Vector3& position, const Vector3& direction) {
	assert(model);
	assert(camera);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.3f, 0.3f, 0.3f}; // 小さいキューブ
	worldTransform_.rotation_.y = std::atan2(direction.x, direction.z);
	UpdateWorldMatrix(worldTransform_);

	velocity_ = {direction.x * kSpeed, 0.0f, direction.z * kSpeed};
}

void Bullet::Update() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.z += velocity_.z;

	if (--lifeTimer_ <= 0) {
		isDead_ = true;
	}

	UpdateWorldMatrix(worldTransform_);
}

void Bullet::Draw() { model_->Draw(worldTransform_, *camera_); }
