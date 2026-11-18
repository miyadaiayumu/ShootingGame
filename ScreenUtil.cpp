#include "ScreenUtil.h"

using namespace KamataEngine;

bool WorldToNDC(const Vector3& p, const Camera& camera, Vector3& ndc) {
	const auto& V = camera.matView.m;
	const auto& P = camera.matProjection.m;

	// ワールド → ビュー（行ベクトル × 行列、w=1）
	float vx = p.x * V[0][0] + p.y * V[1][0] + p.z * V[2][0] + V[3][0];
	float vy = p.x * V[0][1] + p.y * V[1][1] + p.z * V[2][1] + V[3][1];
	float vz = p.x * V[0][2] + p.y * V[1][2] + p.z * V[2][2] + V[3][2];

	// ビュー → クリップ
	float cx = vx * P[0][0] + vy * P[1][0] + vz * P[2][0] + P[3][0];
	float cy = vx * P[0][1] + vy * P[1][1] + vz * P[2][1] + P[3][1];
	float cz = vx * P[0][2] + vy * P[1][2] + vz * P[2][2] + P[3][2];
	float w = vx * P[0][3] + vy * P[1][3] + vz * P[2][3] + P[3][3];

	// カメラの後ろにあるものは画面に映らない（wの確認）
	if (w <= 0.0f) {
		return false;
	}

	// 透視除算
	ndc = {cx / w, cy / w, cz / w};
	return true;
}

bool IsInScreen(const Vector3& worldPos, const Camera& camera, float margin) {
	Vector3 ndc;
	if (!WorldToNDC(worldPos, camera, ndc)) {
		return false;
	}

	const float limit = 1.0f + margin;
	return ndc.x >= -limit && ndc.x <= limit && ndc.y >= -limit && ndc.y <= limit && ndc.z >= 0.0f && ndc.z <= 1.0f;
}
