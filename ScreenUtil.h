#pragma once
#include "KamataEngine.h"

// ワールド座標 → NDC（正規化デバイス座標）
//  戻り値: カメラの後ろ（w <= 0）ならfalse。trueのときだけ ndc が有効
//  DirectXのNDC: x,y は -1〜1、z は 0〜1
bool WorldToNDC(const KamataEngine::Vector3& worldPos, const KamataEngine::Camera& camera, KamataEngine::Vector3& ndc);

// 点（敵の中心など）が画面内にあるか
//  margin: NDC上の余白。0.1 なら画面の外側10%（NDC幅2に対して）まで内側扱い
//          → 画面端で体の一部だけ見えている敵が消えるのを防ぐ
bool IsInScreen(const KamataEngine::Vector3& worldPos, const KamataEngine::Camera& camera, float margin = 0.0f);
