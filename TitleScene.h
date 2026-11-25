#pragma once
#include "Button.h"
#include "KamataEngine.h"

// タイトル画面
class TitleScene {
public:
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();

	// ゲーム開始が選ばれたか
	bool IsFinished() const { return finished_; }

private:
	KamataEngine::Sprite* backgroundSprite_ = nullptr;
	KamataEngine::Sprite* titleSprite_ = nullptr;
	Button* startButton_ = nullptr;

	bool finished_ = false;
};
