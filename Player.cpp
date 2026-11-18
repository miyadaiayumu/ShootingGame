#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "WorldMatrixUtil.h"
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
	UpdateWorldMatrix(mWorldTransform);

	hp_ = kMaxHp;
	invincibleTimer_ = 0;
}

void Player::Reset(const KamataEngine::Vector3& position) {
	mWorldTransform.translation_ = position;
	hp_ = kMaxHp;
	invincibleTimer_ = 0;
	velocity_ = {0.0f, 0.0f, 0.0f};
	UpdateWorldMatrix(mWorldTransform);
}

bool Player::TakeDamage(int damage) {
	// 無敵中・死亡後はダメージを受けない
	if (invincibleTimer_ > 0 || hp_ <= 0) {
		return false;
	}

	hp_ = std::max(hp_ - damage, 0);
	// 倒れたときは点滅させず、そのまま表示しておく
	invincibleTimer_ = (hp_ > 0) ? kInvincibleFrames : 0;
	return true;
}

void Player::Update() {

	// 無敵時間のカウントダウン
	if (invincibleTimer_ > 0) {
		--invincibleTimer_;
	}

	// 入力取得
	Input* input = Input::GetInstance();

	// 移動方向を初期化
	velocity_ = {0.0f, 0.0f, 0.0f};

	// 上下移動（Z軸）
	if (input->PushKey(DIK_W)) {
		velocity_.z += 1.0f;
	}

	if (input->PushKey(DIK_S)) {
		velocity_.z -= 1.0f;
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

	// 座標に移動量を反映（X・Zを別々に判定して壁沿いにスライド）
	Vector3 next = mWorldTransform.translation_;
	next.x += velocity_.x * moveSpeed_;
	if (!mMapChipField || mMapChipField->IsFloor(next)) {
		mWorldTransform.translation_.x = next.x;
	}

	next = mWorldTransform.translation_;
	next.z += velocity_.z * moveSpeed_;
	if (!mMapChipField || mMapChipField->IsFloor(next)) {
		mWorldTransform.translation_.z = next.z;
	}

	// ワールド行列更新
	UpdateWorldMatrix(mWorldTransform);
}

// ===== 描画処理 =====
void Player::Draw() {
	// 無敵中は点滅（4フレームごとに表示/非表示）
	if (invincibleTimer_ > 0 && (invincibleTimer_ / 4) % 2 == 0) {
		return;
	}
	mModel->Draw(mWorldTransform, *mCamera);
}
