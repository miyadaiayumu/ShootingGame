#include "LevelUpMenu.h"

using namespace KamataEngine;

LevelUpMenu::~LevelUpMenu() {
	Close();
	delete dimSprite_;
	dimSprite_ = nullptr;
	delete titleSprite_;
	titleSprite_ = nullptr;
}

void LevelUpMenu::Initialize() {
	const float winW = static_cast<float>(WinApp::kWindowWidth);
	const float winH = static_cast<float>(WinApp::kWindowHeight);

	cardTex_[static_cast<int>(StatType::kShotSpeed)] = TextureManager::Load("ui/card_shot.png");
	cardTex_[static_cast<int>(StatType::kMoveSpeed)] = TextureManager::Load("ui/card_move.png");
	cardTex_[static_cast<int>(StatType::kRange)] = TextureManager::Load("ui/card_range.png");
	cardTex_[static_cast<int>(StatType::kMaxLife)] = TextureManager::Load("ui/card_life.png");

	// 画面を暗くする（白い画像を半透明の黒にして全面に）
	uint32_t whiteTex = TextureManager::Load("ui/white.png");
	dimSprite_ = Sprite::Create(whiteTex, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.65f});
	dimSprite_->SetSize({winW, winH});

	// 「LEVEL UP!」（700x150）
	uint32_t titleTex = TextureManager::Load("ui/levelup_title.png");
	titleSprite_ = Sprite::Create(titleTex, {(winW - 700.0f) * 0.5f, 50.0f});
}

void LevelUpMenu::Open(const std::vector<StatType>& choices) {
	Close();

	const float winW = static_cast<float>(WinApp::kWindowWidth);
	const int n = static_cast<int>(choices.size());
	const float totalW = n * kCardWidth + (n - 1) * kCardGap;
	const float startX = (winW - totalW) * 0.5f;

	for (int i = 0; i < n; ++i) {
		Button* button = new Button();
		button->Initialize(cardTex_[static_cast<int>(choices[i])], {startX + i * (kCardWidth + kCardGap), kCardY}, {kCardWidth, kCardHeight});
		cards_.push_back({choices[i], button});
	}
}

void LevelUpMenu::Close() {
	for (Card& card : cards_) {
		delete card.button;
	}
	cards_.clear();
}

bool LevelUpMenu::Update(StatType& selected) {
	for (Card& card : cards_) {
		card.button->Update();
	}

	for (Card& card : cards_) {
		if (card.button->IsClicked()) {
			selected = card.type;
			Close();
			return true;
		}
	}
	return false;
}

void LevelUpMenu::Draw() {
	if (cards_.empty()) {
		return;
	}

	dimSprite_->Draw();
	titleSprite_->Draw();
	for (Card& card : cards_) {
		card.button->Draw();
	}
}
