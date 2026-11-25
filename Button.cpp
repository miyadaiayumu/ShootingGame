#include "Button.h"
#include "MouseInput.h"

using namespace KamataEngine;

Button::~Button() {
	delete sprite_;
	sprite_ = nullptr;
	delete frameSprite_;
	frameSprite_ = nullptr;
}

void Button::Initialize(uint32_t textureHandle, const Vector2& position, const Vector2& size) {
	position_ = position;
	size_ = size;

	// 枠（白い画像を金色にして、ボタンより少し大きく置く）
	uint32_t whiteTex = TextureManager::Load("ui/white.png");
	frameSprite_ = Sprite::Create(whiteTex, {position.x - kFrameWidth, position.y - kFrameWidth}, {1.0f, 0.85f, 0.3f, 1.0f});
	frameSprite_->SetSize({size.x + kFrameWidth * 2.0f, size.y + kFrameWidth * 2.0f});

	sprite_ = Sprite::Create(textureHandle, position);
	sprite_->SetSize(size);
}

void Button::Update() {
	Vector2 m = MouseInput::GetPosition();
	hovered_ = m.x >= position_.x && m.x <= position_.x + size_.x && m.y >= position_.y && m.y <= position_.y + size_.y;
}

bool Button::IsClicked() const { return hovered_ && MouseInput::IsLeftTriggered(); }

void Button::Draw() {
	if (hovered_) {
		frameSprite_->Draw();
	}
	sprite_->Draw();
}
