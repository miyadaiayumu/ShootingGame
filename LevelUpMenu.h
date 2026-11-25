#pragma once
#include "Button.h"
#include "KamataEngine.h"
#include <vector>

// レベルアップで選べる強化の種類
enum class StatType {
	kShotSpeed, // 連射（発射間隔が短くなる）
	kMoveSpeed, // 移動速度
	kRange,     // 射程
	kMaxLife,   // 最大ライフ
	kCount,
};

// レベルアップ時に出る「強化を選ぶ」画面（カードをクリックして選ぶ）
class LevelUpMenu {
public:
	~LevelUpMenu();

	void Initialize();

	// 選択肢（1〜3個）を表示する
	void Open(const std::vector<StatType>& choices);
	void Close();
	bool IsOpen() const { return !cards_.empty(); }

	// カードがクリックされたら true を返し、selected に選ばれた強化を入れる
	bool Update(StatType& selected);

	// Sprite::PreDraw ～ PostDraw の間で呼ぶ
	void Draw();

private:
	struct Card {
		StatType type;
		Button* button;
	};

	uint32_t cardTex_[static_cast<int>(StatType::kCount)] = {};

	KamataEngine::Sprite* dimSprite_ = nullptr;   // 画面を暗くする
	KamataEngine::Sprite* titleSprite_ = nullptr; // 「LEVEL UP!」
	std::vector<Card> cards_;

	static inline const float kCardWidth = 320.0f;
	static inline const float kCardHeight = 420.0f;
	static inline const float kCardGap = 60.0f;
	static inline const float kCardY = 230.0f;
};
