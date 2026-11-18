#pragma once
#include "KamataEngine.h"

// scale_/rotation_/translation_ から matWorld_ を計算してGPUへ転送する
void UpdateWorldMatrix(KamataEngine::WorldTransform& wt);
