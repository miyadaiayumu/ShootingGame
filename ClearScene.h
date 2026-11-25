#pragma once
#include "Button.h"
#include "KamataEngine.h"

// ステージクリア画面（「もう一度あそぶ」か「タイトルへ」を選ぶ）
class ClearScene {
public:
	enum class Result {
		kNone,    // まだ選んでいない
		kRestart, // もう一度あそぶ
		kTitle,   // タイトルへ
	};

	~ClearScene();

	void Initialize();
	void Update();
	void Draw();

	Result GetResult() const { return result_; }

private:
	KamataEngine::Sprite* backgroundSprite_ = nullptr;
	KamataEngine::Sprite* clearSprite_ = nullptr;
	Button* restartButton_ = nullptr;
	Button* titleButton_ = nullptr;

	Result result_ = Result::kNone;
};
