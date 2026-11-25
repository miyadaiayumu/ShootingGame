#pragma once
#include "Button.h"
#include "KamataEngine.h"

// ゲームオーバー画面（「もう一度あそぶ」か「タイトルへ」を選ぶ）
class GameOverScene {
public:
	enum class Result {
		kNone,    // まだ選んでいない
		kRestart, // もう一度あそぶ
		kTitle,   // タイトルへ
	};

	~GameOverScene();

	void Initialize();
	void Update();
	void Draw();

	Result GetResult() const { return result_; }

private:
	KamataEngine::Sprite* backgroundSprite_ = nullptr;
	KamataEngine::Sprite* gameOverSprite_ = nullptr;
	Button* restartButton_ = nullptr;
	Button* titleButton_ = nullptr;

	Result result_ = Result::kNone;
};
