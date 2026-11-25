#pragma once
#include "KamataEngine.h"

// マウスの位置とクリック（Win32で取得）
//  毎フレーム、メインループで Update() を1回呼ぶこと
class MouseInput {
public:
	static void Update();

	// ウィンドウ内の座標（1280x720 に換算済み）
	static KamataEngine::Vector2 GetPosition();

	// 左クリックが「押された瞬間」か
	static bool IsLeftTriggered();
};
