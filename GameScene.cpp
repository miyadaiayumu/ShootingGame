#include "GameScene.h"
#include "ScreenUtil.h"
#include <assert.h>
#include <cmath>
#include <cstdio>
#include <Windows.h>
#include <imgui.h>
#include <random>

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

	delete mMapChipField;
	mMapChipField = nullptr;

	delete mFloorModel;
	mFloorModel = nullptr;

	for (int i = 0; i < Player::kMaxHp; ++i) {
		delete heartSprites_[i];
		heartSprites_[i] = nullptr;
		delete heartEmptySprites_[i];
		heartEmptySprites_[i] = nullptr;
	}
	delete gameOverSprite_;
	gameOverSprite_ = nullptr;
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

	mMapChipField = new MapChipField();
	mMapChipField->Initialize(&mMainCamera, 4.0f); // 1マスの大きさ
	mMapChipField->SetChipModel(1, mFloorModel);   // ID1 = tile
	if (!mMapChipField->LoadCSV("Resources/mapchip/stage1.csv")) {
		assert(false && "CSVが読み込めません");
	}
	mMapChipField->Generate();

	mPlayer->SetMapChipField(mMapChipField);

	// 敵・弾のモデル（デフォルトのキューブ）
	mEnemyModel = Model::Create();
	mBulletModel = Model::Create();

	// ===== カメラコントロール =====
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(mPlayer);

	float halfW = mMapChipField->GetNumX() * mMapChipField->GetCellSize() * 0.5f;
	float halfH = mMapChipField->GetNumZ() * mMapChipField->GetCellSize() * 0.5f;
	float margin = 8.0f; // 端からどれだけ手前でカメラを止めるか

	CameraController::Rect cameraArea = {
	    -halfW + margin,
	    halfW - margin,
	    -halfH + margin,
	    halfH - margin,
	};
	cameraController_->SetMovableArea(cameraArea);
	cameraController_->Reset();

	// ===== UI（ライフのハート・ゲームオーバー表示） =====
	uint32_t heartTex = TextureManager::Load("ui/heart.png");
	uint32_t heartEmptyTex = TextureManager::Load("ui/heart_empty.png");
	uint32_t gameOverTex = TextureManager::Load("ui/gameover.png");

	for (int i = 0; i < Player::kMaxHp; ++i) {
		Vector2 pos = {20.0f + kHeartSpacing * i, 20.0f};

		heartEmptySprites_[i] = Sprite::Create(heartEmptyTex, pos);
		heartEmptySprites_[i]->SetSize({kHeartSize, kHeartSize});

		heartSprites_[i] = Sprite::Create(heartTex, pos);
		heartSprites_[i]->SetSize({kHeartSize, kHeartSize});
	}

	// 画像は 800x220。画面中央に置く
	gameOverSprite_ = Sprite::Create(gameOverTex, {(WinApp::kWindowWidth - 800.0f) * 0.5f, (WinApp::kWindowHeight - 220.0f) * 0.5f});
}

void GameScene::Update() {
	if (isGameOver_) {
		// ゲームオーバー中は R でリスタート
		if (Input::GetInstance()->TriggerKey(DIK_R)) {
			Restart();
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

	// 敵の出現
	if (++enemySpawnTimer_ >= 25) {
		enemySpawnTimer_ = 0;
		SpawnEnemy();
	}

	// 敵の更新
	for (Enemy* e : enemies_) {
		e->Update(mPlayer->GetWorldPosition());

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

	// ライフが0になったらゲームオーバー
	if (mPlayer->IsDead()) {
		isGameOver_ = true;
	}

	// 自動照準で弾を撃つ
	UpdateAutoShot();

	// 弾の更新
	for (Bullet* b : bullets_) {
		b->Update();
	}

	// 弾と敵の当たり判定
	CheckBulletEnemyCollision();

	// 死んだ敵を削除
	enemies_.remove_if([](Enemy* e) {
		if (e->IsDead()) {
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

#ifdef _DEBUG
	// 60フレームごとに、敵の総数と画面内の数を出力ウィンドウへ表示
	if (++debugPrintTimer_ >= 60) {
		debugPrintTimer_ = 0;

		int visible = 0;
		for (Enemy* e : enemies_) {
			if (IsInScreen(e->GetWorldPosition(), mMainCamera, kDrawMargin)) {
				++visible;
			}
		}

		char buf[128];
		std::snprintf(buf, sizeof(buf), "HP: %d / Enemies: %d (on screen: %d) / Bullets: %d\n", mPlayer->GetHp(), static_cast<int>(enemies_.size()), visible, static_cast<int>(bullets_.size()));
		OutputDebugStringA(buf);
	}
#endif
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

	enemySpawnTimer_ = 0;
	shotTimer_ = 0;

	mPlayer->Reset({0.0f, 0.0f, 0.0f});
	cameraController_->Reset();

	isGameOver_ = false;
}

void GameScene::Draw() {
	Model::PreDraw();

	// 床
	mMapChipField->Draw();

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

	Model::PostDraw();

	// ===== UI（スプライト） =====
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	for (int i = 0; i < Player::kMaxHp; ++i) {
		heartEmptySprites_[i]->Draw();
		if (i < mPlayer->GetHp()) {
			heartSprites_[i]->Draw();
		}
	}

	if (isGameOver_) {
		gameOverSprite_->Draw();
	}

	Sprite::PostDraw();
}

void GameScene::SpawnEnemy() {
	static std::mt19937 rng{std::random_device{}()};

	// 画面の外にある床の上から選ぶ（見つからなければ最後の候補をそのまま使う）
	Vector3 pos{};
	for (int i = 0; i < kSpawnTryCount; ++i) {
		pos = mMapChipField->GetRandomFloorPosition(rng);
		pos.y = 1.0f;
		if (!IsInScreen(pos, mMainCamera, kSpawnMargin)) {
			break;
		}
	}

	Enemy* enemy = new Enemy();
	enemy->Initialize(mEnemyModel, &mMainCamera, pos);
	enemies_.push_back(enemy);
}

void GameScene::UpdateAutoShot() {
	// 発射までの待ち時間
	if (shotTimer_ < kShotInterval) {
		++shotTimer_;
	}
	if (shotTimer_ < kShotInterval) {
		return;
	}

	// 射程内でいちばん近い敵を探す
	const Vector3& playerPos = mPlayer->GetWorldPosition();
	Enemy* nearest = nullptr;
	float nearestDistSq = kShotRange * kShotRange;

	for (Enemy* e : enemies_) {
		if (e->IsDead()) {
			continue;
		}
		const Vector3& ep = e->GetWorldPosition();
		float dx = ep.x - playerPos.x;
		float dz = ep.z - playerPos.z;
		float distSq = dx * dx + dz * dz;
		if (distSq < nearestDistSq) {
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
				break; // この弾は消えたので次の弾へ
			}
		}
	}
}
