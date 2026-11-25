#pragma once
#include "KamataEngine.h"

// クリックできる画像ボタン（マウスが乗ると金色の枠が出る）
class Button {
public:
	~Button();

	void Initialize(uint32_t textureHandle, const KamataEngine::Vector2& position, const KamataEngine::Vector2& size);

	// 毎フレーム、マウスが乗っているかを更新
	void Update();

	// Sprite::PreDraw ～ PostDraw の間で呼ぶ
	void Draw();

	bool IsHovered() const { return hovered_; }
	// マウスが乗っている状態で左クリックされた瞬間
	bool IsClicked() const;

private:
	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Sprite* frameSprite_ = nullptr;

	KamataEngine::Vector2 position_{};
	KamataEngine::Vector2 size_{};
	bool hovered_ = false;

	static inline const float kFrameWidth = 8.0f;
};
