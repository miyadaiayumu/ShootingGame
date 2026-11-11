
#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;

// シーン（型）
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
	kClear,
	kGameOver,
};

Scene scene = Scene::kUnknown;

// 各シーンのポインタ変数
GameScene* gameScene = nullptr;

// 1. シーン切り替え処理
void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
	
		break;

	case Scene::kGame:
		
		break;

	// クリアシーンからの遷移
	case Scene::kClear:
		
		break;

	// ゲームオーバーシーンからの遷移
	case Scene::kGameOver:
		
		break;
	}
}

// 2. 現在のシーンを更新する処理
void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		
		break;

	case Scene::kGame:
		if (gameScene != nullptr) {
			gameScene->Update();
		}
		break;

	case Scene::kClear:
	
		break;

	// ゲームオーバーシーンの更新
	case Scene::kGameOver:
	
		break;
	}
}

// 3. 現在のシーンを描画する処理
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
	
		break;

	case Scene::kGame:
		if (gameScene != nullptr) {
			gameScene->Draw();
		}
		break;

	case Scene::kClear:
	
		break;

	// ゲームオーバーシーンの描画
	case Scene::kGameOver:
	
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// Kamata Engine の初期化
	KamataEngine::Initialize(L"LE2C_25_ミヤダイ_アユム_ピート君のファーストステージ");

	DirectXCommon* dxCommonPtr = DirectXCommon::GetInstance();

	// 最初のシーンの初期化
	scene = Scene::kTitle;

	// メインループ
	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		// シーン切り替え要求のチェック
		ChangeScene();

		// 現在アクティブなシーンの更新
		UpdateScene();

		// 描画ここから
		dxCommonPtr->PreDraw();

		// 現在アクティブなシーンの描画
		DrawScene();

		// 描画ここまで
		dxCommonPtr->PostDraw();
	}

	// 後始末
	if (gameScene != nullptr) {
		delete gameScene;
		gameScene = nullptr;
	}

	KamataEngine::Finalize();

	return 0;
}