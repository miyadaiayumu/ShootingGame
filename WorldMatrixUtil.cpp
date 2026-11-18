#include "WorldMatrixUtil.h"
#include "Matrix4x4.h"
#include "Vector3.h"

void UpdateWorldMatrix(KamataEngine::WorldTransform& wt) {
	::Matrix4x4 m = ::Matrix4x4::MakeAffineMatrix(
	    ::Vector3(wt.scale_.x, wt.scale_.y, wt.scale_.z),
	    ::Vector3(wt.rotation_.x, wt.rotation_.y, wt.rotation_.z),
	    ::Vector3(wt.translation_.x, wt.translation_.y, wt.translation_.z));

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			wt.matWorld_.m[i][j] = m.m[i][j];
		}
	}
	wt.TransferMatrix();
}
