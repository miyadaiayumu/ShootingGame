
#include "ClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "MouseInput.h"
#include "TitleScene.h"
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
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;
ClearScene* clearScene = nullptr;
GameOverScene* gameOverScene = nullptr;

// タイトルシーンを作って切り替える
void StartTitle() {
	titleScene = new TitleScene();
	titleScene->Initialize();
	scene = Scene::kTitle;
}

// ゲームシーンを作って切り替える
void StartGame() {
	gameScene = new GameScene();
	gameScene->Initialize();
	scene = Scene::kGame;
}

// 1. シーン切り替え処理
void ChangeScene() {
	switch (scene) {

	case Scene::kTitle:
		// スタートが押されたらゲームへ
		if (titleScene != nullptr && titleScene->IsFinished()) {
			delete titleScene;
			titleScene = nullptr;
			StartGame();
		}
		break;

	case Scene::kGame:
		if (gameScene == nullptr) {
			break;
		}
		// ゲームをクリアしたらクリア画面へ
		if (gameScene->IsClearFinished()) {
			delete gameScene;
			gameScene = nullptr;

			clearScene = new ClearScene();
			clearScene->Initialize();
			scene = Scene::kClear;
		}
		// ライフが尽きたらゲームオーバー画面へ
		else if (gameScene->IsGameOverFinished()) {
			delete gameScene;
			gameScene = nullptr;

			gameOverScene = new GameOverScene();
			gameOverScene->Initialize();
			scene = Scene::kGameOver;
		}
		break;

	case Scene::kClear:
		// クリア画面で選ばれた行き先へ
		if (clearScene != nullptr && clearScene->GetResult() != ClearScene::Result::kNone) {
			ClearScene::Result result = clearScene->GetResult();

			delete clearScene;
			clearScene = nullptr;

			if (result == ClearScene::Result::kRestart) {
				StartGame();
			} else {
				StartTitle();
			}
		}
		break;

	case Scene::kGameOver:
		// ゲームオーバー画面で選ばれた行き先へ
		if (gameOverScene != nullptr && gameOverScene->GetResult() != GameOverScene::Result::kNone) {
			GameOverScene::Result result = gameOverScene->GetResult();

			delete gameOverScene;
			gameOverScene = nullptr;

			if (result == GameOverScene::Result::kRestart) {
				StartGame();
			} else {
				StartTitle();
			}
		}
		break;
	}
}

// 2. 現在のシーンを更新する処理
void UpdateScene() {
	switch (scene) {

	case Scene::kTitle:
		if (titleScene != nullptr) {
			titleScene->Update();
		}
		break;

	case Scene::kGame:
		if (gameScene != nullptr) {
			gameScene->Update();
		}
		break;

	case Scene::kClear:
		if (clearScene != nullptr) {
			clearScene->Update();
		}
		break;

	case Scene::kGameOver:
		if (gameOverScene != nullptr) {
			gameOverScene->Update();
		}
		break;
	}
}

// 3. 現在のシーンを描画する処理
void DrawScene() {
	switch (scene) {

	case Scene::kTitle:
		if (titleScene != nullptr) {
			titleScene->Draw();
		}
		break;

	case Scene::kGame:
		if (gameScene != nullptr) {
			gameScene->Draw();
		}
		break;

	case Scene::kClear:
		if (clearScene != nullptr) {
			clearScene->Draw();
		}
		break;

	case Scene::kGameOver:
		if (gameOverScene != nullptr) {
			gameOverScene->Draw();
		}
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// Kamata Engine の初期化
	KamataEngine::Initialize(L"LE2C_28_ミヤダイ_アユム");

	DirectXCommon* dxCommonPtr = DirectXCommon::GetInstance();

	// 最初のシーン（タイトル）
	StartTitle();

	// メインループ
	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		// マウスの状態を更新（1フレームに1回）
		MouseInput::Update();

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
	if (titleScene != nullptr) {
		delete titleScene;
		titleScene = nullptr;
	}
	if (gameScene != nullptr) {
		delete gameScene;
		gameScene = nullptr;
	}
	if (clearScene != nullptr) {
		delete clearScene;
		clearScene = nullptr;
	}
	if (gameOverScene != nullptr) {
		delete gameOverScene;
		gameOverScene = nullptr;
	}

	KamataEngine::Finalize();

	return 0;
}
