#include "Enemy.h"
#include "MapChipField.h"
#include "WorldMatrixUtil.h"
#include <algorithm>
#include <assert.h>
#include <cmath>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.scale_ = {kModelScale, kModelScale, kModelScale};
	// 高さは出現位置のyではなく、足元が床に付くように固定する
	worldTransform_.translation_ = {position.x, kGroundOffsetY, position.z};
	UpdateWorldMatrix(worldTransform_);
}

void Enemy::Update(const Vector3& targetPos, const MapChipField* field) {
	// 倒されたあとは、動かずにやられ演出だけ
	if (isDead_) {
		UpdateDeath();
		return;
	}

	const Vector3& pos = worldTransform_.translation_;

	// プレイヤーへの方向（XZ平面のみ）
	float dx = targetPos.x - pos.x;
	float dz = targetPos.z - pos.z;
	float len = std::sqrt(dx * dx + dz * dz);

	if (len > 0.001f) {
		// 基本はまっすぐプレイヤーへ
		Vector3 dir = {dx / len, 0.0f, dz / len};

		// 穴が間にあってまっすぐ行けないときは、穴を避ける経路に従う
		if (field && !field->HasLineOfSight(pos, targetPos)) {
			Vector3 flowDir;
			if (field->GetFlowDirection(pos, flowDir)) {
				dir = flowDir;
			}
		}

		// X・Zを別々に動かし、穴（床でない場所）には入らない
		Vector3 next = worldTransform_.translation_;
		next.x += dir.x * moveSpeed_;
		if (!field || field->IsFloor(next)) {
			worldTransform_.translation_.x = next.x;
		}
		next = worldTransform_.translation_;
		next.z += dir.z * moveSpeed_;
		if (!field || field->IsFloor(next)) {
			worldTransform_.translation_.z = next.z;
		}

		// 進行方向を向く（モデルの正面が+Zの場合。ズレるならオフセットを足す）
		worldTransform_.rotation_.y = std::atan2(dir.x, dir.z) + kModelYawOffset;
	}

	UpdateWorldMatrix(worldTransform_);
}

void Enemy::UpdateDeath() {
	++deathTimer_;
	float t = (std::min)(static_cast<float>(deathTimer_) / static_cast<float>(kDeathFrames), 1.0f);

	// 最初の3割でぷくっと膨らみ、残りで一気に縮む
	float scale = (t < 0.3f) ? (1.0f + t) : (1.3f * (1.0f - t) / 0.7f);
	float s = kModelScale * scale;
	worldTransform_.scale_ = {s, s, s};

	// 回転しながら、少し浮く
	worldTransform_.rotation_.y += 0.6f;
	worldTransform_.translation_.y += 0.05f;

	UpdateWorldMatrix(worldTransform_);
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }
