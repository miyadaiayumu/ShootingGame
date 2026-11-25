#include "GameScene.h"
#include "MouseInput.h"
#include "ScreenUtil.h"
#include "WorldMatrixUtil.h"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <cstdio>
#include <random>
#include <Windows.h>

using namespace KamataEngine;

GameScene::~GameScene() {

	delete mPlayer;
	mPlayer = nullptr;

	delete mPlayerModel;
	mPlayerModel = nullptr;

	delete mDebugCamera;
	mDebugCamera = nullptr;

	delete cameraController_;
	cameraController_ = nullptr;

	for (Enemy* e : enemies_) {
		delete e;
	}
	enemies_.clear();
	delete mEnemyModel;
	mEnemyModel = nullptr;

	for (Bullet* b : bullets_) {
		delete b;
	}
	bullets_.clear();
	delete mBulletModel;
	mBulletModel = nullptr;

	for (Drop* d : drops_) {
		delete d;
	}
	drops_.clear();
	delete mDropModel;
	mDropModel = nullptr;

	delete deathEffect_;
	deathEffect_ = nullptr;
	delete mParticleModel;
	mParticleModel = nullptr;
	delete playerDeathEffect_;
	playerDeathEffect_ = nullptr;
	delete mPlayerDeathModel;
	mPlayerDeathModel = nullptr;
	delete hitEffect_;
	hitEffect_ = nullptr;
	delete mHitModel;
	mHitModel = nullptr;
	delete trailEffect_;
	trailEffect_ = nullptr;
	delete mTrailModel;
	mTrailModel = nullptr;

	delete levelUpMenu_;
	levelUpMenu_ = nullptr;

	delete mMapChipField;
	mMapChipField = nullptr;

	delete mFloorModel;
	mFloorModel = nullptr;
	delete mWallModel;
	mWallModel = nullptr;

	for (int i = 0; i < Player::kMaxHpLimit; ++i) {
		delete heartSprites_[i];
		heartSprites_[i] = nullptr;
		delete heartEmptySprites_[i];
		heartEmptySprites_[i] = nullptr;
	}
	delete timeLabelSprite_;
	timeLabelSprite_ = nullptr;
	delete timeGaugeBackSprite_;
	timeGaugeBackSprite_ = nullptr;
	delete timeGaugeFillSprite_;
	timeGaugeFillSprite_ = nullptr;

	delete expLabelSprite_;
	expLabelSprite_ = nullptr;
	delete expGaugeBackSprite_;
	expGaugeBackSprite_ = nullptr;
	delete expGaugeFillSprite_;
	expGaugeFillSprite_ = nullptr;
}

void GameScene::Initialize() {
	// カメラ初期化
	mMainCamera.Initialize();

	// farZを伸ばす
	mMainCamera.farZ = 1000.0f;

	// 行列更新
	mMainCamera.UpdateMatrix();

	mDebugCamera = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// プレイヤーモデル読み込み
	mPlayerModel = Model::CreateFromOBJ("Player");

	// プレイヤー生成
	mPlayer = new Player();

	// プレイヤー初期化
	mPlayer->Initialize(mPlayerModel, &mMainCamera, {0.0f, 0.0f, 0.0f});

	// 床（マップチップ）
	mFloorModel = Model::CreateFromOBJ("tile");
	mWallModel = Model::CreateFromOBJ("wall");

	mMapChipField = new MapChipField();
	mMapChipField->Initialize(&mMainCamera, 4.0f); // 1マスの大きさ
	mMapChipField->SetChipModel(MapChipField::kChipFloor, mFloorModel); // ID1 = 床
	mMapChipField->SetChipModel(MapChipField::kChipWall, mWallModel);   // ID2 = 壁
	mMapChipField->SetChipHeightScale(MapChipField::kChipWall, kWallHeightScale);
	if (!mMapChipField->LoadCSV("Resources/mapchip/stage1.csv")) {
		assert(false && "CSVが読み込めません");
	}
	mMapChipField->Generate();

	mPlayer->SetMapChipField(mMapChipField);

	// 敵・弾・ドロップのモデル（デフォルトのキューブ）
	mEnemyModel = Model::CreateFromOBJ("enemy"); // 敵のモデル
	mBulletModel = Model::CreateFromOBJ("bullet"); // 細長い弾
	mDropModel = Model::Create();

	// 敵を倒したときの破片（赤〜オレンジ）
	mParticleModel = Model::CreateFromOBJ("effect_enemy");
	DeathEffect::Config enemyConfig;
	enemyConfig.poolSize = 300;
	enemyConfig.burstCount = 10;
	deathEffect_ = new DeathEffect();
	deathEffect_->Initialize(mParticleModel, &mMainCamera, enemyConfig);

	// プレイヤーが倒れたときの派手な破片（白〜水色）
	mPlayerDeathModel = Model::CreateFromOBJ("effect_player");
	DeathEffect::Config playerConfig;
	playerConfig.poolSize = 60;
	playerConfig.burstCount = 30;
	playerConfig.speedMin = 0.15f;
	playerConfig.speedMax = 0.5f;
	playerConfig.upMin = 0.25f;
	playerConfig.upMax = 0.55f;
	playerConfig.lifeMin = 30;
	playerConfig.lifeMax = 55;
	playerConfig.scaleMin = 0.25f;
	playerConfig.scaleMax = 0.5f;
	playerDeathEffect_ = new DeathEffect();
	playerDeathEffect_->Initialize(mPlayerDeathModel, &mMainCamera, playerConfig);

	// 弾が壁に当たったときの火花（黄色）
	mHitModel = Model::CreateFromOBJ("effect_hit");
	DeathEffect::Config hitConfig;
	hitConfig.poolSize = 80;
	hitConfig.burstCount = 5;
	hitConfig.speedMin = 0.08f;
	hitConfig.speedMax = 0.22f;
	hitConfig.upMin = 0.1f;
	hitConfig.upMax = 0.25f;
	hitConfig.lifeMin = 8;
	hitConfig.lifeMax = 16;
	hitConfig.scaleMin = 0.12f;
	hitConfig.scaleMax = 0.22f;
	hitEffect_ = new DeathEffect();
	hitEffect_->Initialize(mHitModel, &mMainCamera, hitConfig);

	// 弾の軌跡（浮いたまま小さくなって消える）
	mTrailModel = Model::CreateFromOBJ("effect_trail");
	DeathEffect::Config trailConfig;
	trailConfig.poolSize = 200;
	trailConfig.burstCount = 1;
	trailConfig.speedMin = 0.0f;
	trailConfig.speedMax = 0.03f;
	trailConfig.upMin = 0.0f;
	trailConfig.upMax = 0.02f;
	trailConfig.gravity = 0.0f;
	trailConfig.lifeMin = 8;
	trailConfig.lifeMax = 12;
	trailConfig.scaleMin = 0.18f;
	trailConfig.scaleMax = 0.26f;
	trailConfig.spinMax = 0.1f;
	trailEffect_ = new DeathEffect();
	trailEffect_->Initialize(mTrailModel, &mMainCamera, trailConfig);

	// ===== カメラコントロール =====
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(mPlayer);

	float halfW = mMapChipField->GetNumX() * mMapChipField->GetCellSize() * 0.5f;
	float halfH = mMapChipField->GetNumZ() * mMapChipField->GetCellSize() * 0.5f;
	float margin = 6.0f; // 端からどれだけ手前でカメラを止めるか

	CameraController::Rect cameraArea = {
	    -halfW + margin,
	    halfW - margin,
	    -halfH + margin,
	    halfH - margin,
	};
	cameraController_->SetMovableArea(cameraArea);
	cameraController_->Reset();

	// ===== UI =====
	const float winW = static_cast<float>(WinApp::kWindowWidth);
	const float winH = static_cast<float>(WinApp::kWindowHeight);

	uint32_t heartTex = TextureManager::Load("ui/heart.png");
	uint32_t heartEmptyTex = TextureManager::Load("ui/heart_empty.png");
	uint32_t whiteTex = TextureManager::Load("ui/white.png");
	uint32_t timeLabelTex = TextureManager::Load("ui/time_label.png");
	uint32_t expLabelTex = TextureManager::Load("ui/exp_label.png");

	// ライフのハート（最大ライフの上限ぶんだけ用意しておく）
	for (int i = 0; i < Player::kMaxHpLimit; ++i) {
		Vector2 pos = {20.0f + kHeartSpacing * i, 20.0f};

		heartEmptySprites_[i] = Sprite::Create(heartEmptyTex, pos);
		heartEmptySprites_[i]->SetSize({kHeartSize, kHeartSize});

		heartSprites_[i] = Sprite::Create(heartTex, pos);
		heartSprites_[i]->SetSize({kHeartSize, kHeartSize});
	}

	// 生き残り時間ゲージ（白い画像を色付きで伸縮させて使う）
	float gaugeX = (winW - kGaugeWidth) * 0.5f;
	float gaugeY = 28.0f;
	timeLabelSprite_ = Sprite::Create(timeLabelTex, {gaugeX - 110.0f, gaugeY - 6.0f});
	timeGaugeBackSprite_ = Sprite::Create(whiteTex, {gaugeX, gaugeY}, {0.0f, 0.0f, 0.0f, 0.6f});
	timeGaugeBackSprite_->SetSize({kGaugeWidth, kGaugeHeight});
	timeGaugeFillSprite_ = Sprite::Create(whiteTex, {gaugeX, gaugeY}, {0.4f, 0.9f, 0.5f, 1.0f});
	timeGaugeFillSprite_->SetSize({1.0f, kGaugeHeight});

	// 経験値ゲージ（画面下）
	float expX = (winW - kExpGaugeWidth) * 0.5f;
	float expY = winH - 44.0f;
	expLabelSprite_ = Sprite::Create(expLabelTex, {expX - 100.0f, expY - 10.0f});
	expGaugeBackSprite_ = Sprite::Create(whiteTex, {expX, expY}, {0.0f, 0.0f, 0.0f, 0.6f});
	expGaugeBackSprite_->SetSize({kExpGaugeWidth, kExpGaugeHeight});
	expGaugeFillSprite_ = Sprite::Create(whiteTex, {expX, expY}, {0.3f, 0.7f, 1.0f, 1.0f});
	expGaugeFillSprite_->SetSize({1.0f, kExpGaugeHeight});

	// レベルアップ選択画面
	levelUpMenu_ = new LevelUpMenu();
	levelUpMenu_->Initialize();

	// 最初のウェーブまでの待ち時間
	waveRestTimer_ = kWaveInterval - kFirstWaveDelay;
}

void GameScene::Update() {
	if (isCleared_) {
		// クリア後は少し待ってから、main側でクリア画面へ切り替わる
		++clearTimer_;
		UpdateEffects();
	} else if (isGameOver_) {
		// ゲームオーバー後は少し待ってから、main側でゲームオーバー画面へ切り替わる
		++gameOverTimer_;
		UpdateEffects(); // プレイヤーの死亡エフェクトを最後まで見せる
	} else if (isLevelUp_) {
		// レベルアップ中：ゲームを止めて、カードのクリックを待つ
		StatType selected;
		if (levelUpMenu_->Update(selected)) {
			ApplyStat(selected);
			isLevelUp_ = false;
		}
	} else {
		UpdateGameplay();
	}

	// --- カメラの更新処理 ---
	bool isDebugMode = false;

#ifdef _DEBUG
	Input* input = Input::GetInstance();

	if (input->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	isDebugMode = isDebugCameraActive_;
#endif

	if (isDebugMode && mDebugCamera) {
		mDebugCamera->Update();
		mMainCamera.matView = mDebugCamera->GetCamera().matView;
		mMainCamera.matProjection = mDebugCamera->GetCamera().matProjection;
		mMainCamera.TransferMatrix();
	} else {
		mMainCamera.matView = cameraController_->GetCamera().matView;
		mMainCamera.matProjection = cameraController_->GetCamera().matProjection;
		mMainCamera.translation_ = cameraController_->GetCamera().translation_;
		mMainCamera.rotation_ = cameraController_->GetCamera().rotation_;
		mMainCamera.TransferMatrix();
	}
}

void GameScene::UpdateGameplay() {

	mPlayer->Update();

	cameraController_->Update();

	// ウェーブ（敵の出現）
	UpdateWaves();

	// 敵が穴を避けて進めるよう、プレイヤーのいる場所からの経路を更新
	mMapChipField->BuildFlowField(mPlayer->GetWorldPosition());

	// 敵の更新
	for (Enemy* e : enemies_) {
		e->Update(mPlayer->GetWorldPosition(), mMapChipField);

		// プレイヤーとの接触：ダメージを受け、敵は消える（無敵中はすり抜ける）
		if (e->IsDead() || mPlayer->IsInvincible()) {
			continue;
		}
		const Vector3& p = mPlayer->GetWorldPosition();
		const Vector3& ep = e->GetWorldPosition();
		float dx = p.x - ep.x, dz = p.z - ep.z;
		if (dx * dx + dz * dz < 1.5f * 1.5f) {
			if (mPlayer->TakeDamage()) {
				e->Kill();
			}
		}
	}

	// ライフが0になったらゲームオーバー（プレイヤーが砕け散る）
	if (mPlayer->IsDead()) {
		isGameOver_ = true;
		const Vector3& pp = mPlayer->GetWorldPosition();
		playerDeathEffect_->Emit({pp.x, 1.0f, pp.z});
	}

	// 自動照準で弾を撃つ
	UpdateAutoShot();

	// 弾の更新（壁に当たったら火花を出して消える。飛んでいる間は軌跡を残す）
	trailTimer_ = (trailTimer_ + 1) % 2; // 2フレームに1回、軌跡を出す
	for (Bullet* b : bullets_) {
		b->Update();
		const Vector3& bp = b->GetWorldPosition();
		if (mMapChipField->IsWall(bp)) {
			hitEffect_->Emit(bp);
			b->Kill();
		} else if (trailTimer_ == 0) {
			trailEffect_->Emit(bp);
		}
	}

	// 弾と敵の当たり判定（倒した敵の位置にドロップが出る）
	CheckBulletEnemyCollision();

	// 一定時間生き残ったらクリア
	if (++survivalTimer_ >= kClearTimeFrames) {
		isCleared_ = true;
	}

	// エフェクトの更新
	UpdateEffects();

	// ドロップの更新・回収
	UpdateDrops();

	// やられ演出が終わった敵を削除
	enemies_.remove_if([](Enemy* e) {
		if (e->IsRemovable()) {
			delete e;
			return true;
		}
		return false;
	});

	// 死んだ弾を削除
	bullets_.remove_if([](Bullet* b) {
		if (b->IsDead()) {
			delete b;
			return true;
		}
		return false;
	});

	// 経験値がたまっていたらレベルアップ
	if (!isGameOver_ && !isCleared_) {
		CheckLevelUp();
	}

#ifdef _DEBUG
	// 60フレームごとに状態を出力ウィンドウへ表示
	if (++debugPrintTimer_ >= 60) {
		debugPrintTimer_ = 0;

		int visible = 0;
		for (Enemy* e : enemies_) {
			if (IsInScreen(e->GetWorldPosition(), mMainCamera, kDrawMargin)) {
				++visible;
			}
		}

		char buf[200];
		std::snprintf(
		    buf, sizeof(buf), "HP: %d/%d / Wave: %d / Time: %d/%d s / Kill: %d / Lv: %d (EXP %d/%d) / Enemies: %d (on screen: %d) / Bullets: %d\n", mPlayer->GetHp(), mPlayer->GetMaxHp(), wave_, survivalTimer_ / 60, kClearTimeSeconds, killCount_, level_, exp_,
		    GetExpToNext(), static_cast<int>(enemies_.size()), visible, static_cast<int>(bullets_.size()));
		OutputDebugStringA(buf);
	}
#endif
}

void GameScene::UpdateEffects() {
	deathEffect_->Update();
	playerDeathEffect_->Update();
	hitEffect_->Update();
	trailEffect_->Update();
}

void GameScene::Restart() {
	for (Enemy* e : enemies_) {
		delete e;
	}
	enemies_.clear();

	for (Bullet* b : bullets_) {
		delete b;
	}
	bullets_.clear();

	for (Drop* d : drops_) {
		delete d;
	}
	drops_.clear();

	deathEffect_->Clear();
	playerDeathEffect_->Clear();
	hitEffect_->Clear();
	trailEffect_->Clear();

	// ウェーブを最初から
	wave_ = 0;
	waveEnemiesLeft_ = 0;
	waveSpawnTimer_ = 0;
	waveRestTimer_ = kWaveInterval - kFirstWaveDelay;

	shotTimer_ = 0;

	// 強化を初期状態へ
	shotInterval_ = kBaseShotInterval;
	shotRange_ = kBaseShotRange;
	for (int& lv : statLevels_) {
		lv = 0;
	}
	level_ = 1;
	exp_ = 0;
	levelUpMenu_->Close();
	isLevelUp_ = false;

	mPlayer->Reset({0.0f, 0.0f, 0.0f});
	cameraController_->Reset();

	killCount_ = 0;
	survivalTimer_ = 0;
	isGameOver_ = false;
	isCleared_ = false;
	clearTimer_ = 0;
	gameOverTimer_ = 0;
}

void GameScene::Draw() {
	Model::PreDraw();

	// 床
	mMapChipField->Draw();

	// ドロップ
	for (Drop* d : drops_) {
		if (IsInScreen(d->GetWorldPosition(), mMainCamera, kDrawMargin)) {
			d->Draw();
		}
	}

	// プレイヤー
	mPlayer->Draw();

	// 敵（画面内のものだけ描画。体の一部が見えている敵を消さないよう余白を付ける）
	for (Enemy* e : enemies_) {
		if (IsInScreen(e->GetWorldPosition(), mMainCamera, kDrawMargin)) {
			e->Draw();
		}
	}

	// 弾
	for (Bullet* b : bullets_) {
		if (IsInScreen(b->GetWorldPosition(), mMainCamera, kDrawMargin)) {
			b->Draw();
		}
	}

	// エフェクト
	trailEffect_->Draw();
	hitEffect_->Draw();
	deathEffect_->Draw();
	playerDeathEffect_->Draw();

	Model::PostDraw();

	// ===== UI（スプライト） =====
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// ライフ（最大ライフぶんの枠と、残りライフのハート）
	for (int i = 0; i < mPlayer->GetMaxHp(); ++i) {
		heartEmptySprites_[i]->Draw();
		if (i < mPlayer->GetHp()) {
			heartSprites_[i]->Draw();
		}
	}

	// 生き残り時間ゲージ
	float timeRatio = static_cast<float>(survivalTimer_) / static_cast<float>(kClearTimeFrames);
	timeRatio = (std::min)(timeRatio, 1.0f);
	timeGaugeFillSprite_->SetSize({(std::max)(kGaugeWidth * timeRatio, 1.0f), kGaugeHeight});

	timeLabelSprite_->Draw();
	timeGaugeBackSprite_->Draw();
	timeGaugeFillSprite_->Draw();

	// 経験値ゲージ
	float expRatio = static_cast<float>(exp_) / static_cast<float>(GetExpToNext());
	expRatio = (std::min)(expRatio, 1.0f);
	expGaugeFillSprite_->SetSize({(std::max)(kExpGaugeWidth * expRatio, 1.0f), kExpGaugeHeight});

	expLabelSprite_->Draw();
	expGaugeBackSprite_->Draw();
	expGaugeFillSprite_->Draw();

	// レベルアップ選択画面（いちばん手前）
	if (isLevelUp_) {
		levelUpMenu_->Draw();
	}

	Sprite::PostDraw();
}

void GameScene::UpdateWaves() {
	if (waveEnemiesLeft_ > 0) {
		// ウェーブ中：敵を1体ずつ、同じ方向から立て続けに出す
		if (++waveSpawnTimer_ >= kWaveSpawnDelay) {
			waveSpawnTimer_ = 0;
			SpawnEnemy(waveAngle_, kWaveSpread);
			--waveEnemiesLeft_;
		}
	} else {
		// 休憩中：時間がたったら次のウェーブを始める
		if (++waveRestTimer_ >= kWaveInterval) {
			waveRestTimer_ = 0;
			++wave_;
			waveEnemiesLeft_ = GetWaveEnemyCount();

			// 今回のウェーブが来る方向をランダムに決める
			static std::mt19937 rng{std::random_device{}()};
			std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
			waveAngle_ = angleDist(rng);
		}
	}
}

void GameScene::SpawnEnemy(float baseAngle, float spread) {
	static std::mt19937 rng{std::random_device{}()};
	std::uniform_real_distribution<float> spreadDist(-spread, spread);
	std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
	std::uniform_real_distribution<float> distDist(kSpawnMinDist, kSpawnMaxDist);

	// プレイヤーの周りで、床の上かつ画面の外の位置を探す
	const Vector3& pp = mPlayer->GetWorldPosition();

	for (int i = 0; i < kSpawnTryCount; ++i) {
		// 前半はウェーブの方向、見つからなければ全方向から探す
		float angle = (i < kSpawnTryCount / 2) ? baseAngle + spreadDist(rng) : angleDist(rng);
		float dist = distDist(rng);
		Vector3 pos = {pp.x + std::cos(angle) * dist, 1.0f, pp.z + std::sin(angle) * dist};

		if (!mMapChipField->IsFloor(pos)) {
			continue;
		}
		if (IsInScreen(pos, mMainCamera, kSpawnMargin)) {
			continue;
		}

		Enemy* enemy = new Enemy();
		enemy->Initialize(mEnemyModel, &mMainCamera, pos);
		enemies_.push_back(enemy);
		return;
	}
	// 見つからなければ今回は出現しない
}

void GameScene::UpdateAutoShot() {
	// 発射までの待ち時間
	if (shotTimer_ < shotInterval_) {
		++shotTimer_;
	}
	if (shotTimer_ < shotInterval_) {
		return;
	}

	// 射程内でいちばん近い敵を探す
	const Vector3& playerPos = mPlayer->GetWorldPosition();
	Enemy* nearest = nullptr;
	float nearestDistSq = shotRange_ * shotRange_;

	for (Enemy* e : enemies_) {
		if (e->IsDead()) {
			continue;
		}
		const Vector3& ep = e->GetWorldPosition();
		float dx = ep.x - playerPos.x;
		float dz = ep.z - playerPos.z;
		float distSq = dx * dx + dz * dz;
		if (distSq < nearestDistSq && mMapChipField->HasClearShot(playerPos, ep)) { // 壁越しは狙わない
			nearestDistSq = distSq;
			nearest = e;
		}
	}

	// 狙える敵がいなければ撃たない（タイマーは満タンのまま待機）
	if (!nearest) {
		return;
	}

	// 敵への方向（単位ベクトル）
	const Vector3& tp = nearest->GetWorldPosition();
	float dx = tp.x - playerPos.x;
	float dz = tp.z - playerPos.z;
	float len = std::sqrt(dx * dx + dz * dz);
	if (len < 0.001f) {
		return;
	}
	Vector3 dir = {dx / len, 0.0f, dz / len};

	// 弾を生成
	Vector3 spawnPos = {playerPos.x, 1.0f, playerPos.z};
	Bullet* bullet = new Bullet();
	bullet->Initialize(mBulletModel, &mMainCamera, spawnPos, dir);
	bullets_.push_back(bullet);

	shotTimer_ = 0;
}

void GameScene::CheckBulletEnemyCollision() {
	const float kHitRadius = 1.3f; // 弾と敵の中心がこれ以内で命中

	for (Bullet* b : bullets_) {
		if (b->IsDead()) {
			continue;
		}
		const Vector3& bp = b->GetWorldPosition();

		for (Enemy* e : enemies_) {
			if (e->IsDead()) {
				continue;
			}
			const Vector3& ep = e->GetWorldPosition();
			float dx = bp.x - ep.x;
			float dz = bp.z - ep.z;
			if (dx * dx + dz * dz < kHitRadius * kHitRadius) {
				b->Kill();
				e->Kill();
				++killCount_;
				deathEffect_->Emit({ep.x, 1.0f, ep.z}); // 破片が飛び散る
				SpawnDrop(ep);                          // 倒した場所にドロップ
				break;         // この弾は消えたので次の弾へ
			}
		}
	}
}

void GameScene::SpawnDrop(const Vector3& position) {
	Drop* drop = new Drop();
	drop->Initialize(mDropModel, &mMainCamera, position);
	drops_.push_back(drop);
}

void GameScene::UpdateDrops() {
	for (Drop* d : drops_) {
		d->Update(mPlayer->GetWorldPosition(), kMagnetRange);
		if (d->IsCollected()) {
			++exp_; // ドロップ1個 = 経験値1
		}
	}

	drops_.remove_if([](Drop* d) {
		if (d->IsCollected()) {
			delete d;
			return true;
		}
		return false;
	});
}

void GameScene::CheckLevelUp() {
	if (exp_ < GetExpToNext()) {
		return;
	}

	exp_ -= GetExpToNext();
	++level_;

	// まだ強化できる項目を集める
	std::vector<StatType> available;
	for (int i = 0; i < static_cast<int>(StatType::kCount); ++i) {
		if (statLevels_[i] < kStatMaxLevels[i]) {
			available.push_back(static_cast<StatType>(i));
		}
	}
	// 全部上限なら選択画面は出さない
	if (available.empty()) {
		return;
	}

	// ランダムに最大3つ選ぶ
	static std::mt19937 rng{std::random_device{}()};
	std::shuffle(available.begin(), available.end(), rng);
	if (available.size() > 3) {
		available.resize(3);
	}

	levelUpMenu_->Open(available);
	isLevelUp_ = true;
}

void GameScene::ApplyStat(StatType type) {
	++statLevels_[static_cast<int>(type)];

	switch (type) {
	case StatType::kShotSpeed:
		// 発射間隔を15%短くする（下限あり）
		shotInterval_ = (std::max)(kMinShotInterval, static_cast<int>(shotInterval_ * 0.85f));
		break;
	case StatType::kMoveSpeed:
		mPlayer->AddMoveSpeed(kMoveSpeedUpAmount);
		break;
	case StatType::kRange:
		shotRange_ += kRangeUpAmount;
		break;
	case StatType::kMaxLife:
		mPlayer->AddMaxHp();
		break;
	default:
		break;
	}
}
