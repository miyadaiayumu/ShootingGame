#include "MouseInput.h"
#include <Windows.h>

using namespace KamataEngine;

namespace {
Vector2 sPosition = {0.0f, 0.0f};
bool sCurrent = false;
bool sPrevious = false;
} // namespace

void MouseInput::Update() {
	sPrevious = sCurrent;

	// ゲームウィンドウがアクティブでなければクリックを無視
	HWND hwnd = GetActiveWindow();
	if (!hwnd) {
		sCurrent = false;
		return;
	}

	POINT pt{};
	GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);

	// ウィンドウのクライアント領域の大きさが 1280x720 と違っても合うように換算
	RECT rc{};
	GetClientRect(hwnd, &rc);
	float w = static_cast<float>(rc.right - rc.left);
	float h = static_cast<float>(rc.bottom - rc.top);
	if (w > 0.0f && h > 0.0f) {
		sPosition.x = pt.x * (static_cast<float>(WinApp::kWindowWidth) / w);
		sPosition.y = pt.y * (static_cast<float>(WinApp::kWindowHeight) / h);
	}

	sCurrent = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
}

Vector2 MouseInput::GetPosition() { return sPosition; }

bool MouseInput::IsLeftTriggered() { return sCurrent && !sPrevious; }
