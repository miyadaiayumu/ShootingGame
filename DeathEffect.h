#pragma once
#include "KamataEngine.h"
#include <memory>
#include <vector>

// 破片・火花のエフェクト（あらかじめ用意した粒を使い回す）
//  設定(Config)を変えると、敵の撃破・プレイヤーの死亡・弾の命中・弾の軌跡など、いろいろな見た目になる
class DeathEffect {
public:
	struct Config {
		int poolSize = 300;     // 同時に出せる粒の数
		int burstCount = 10;    // 1回の Emit で出る粒の数
		float speedMin = 0.12f; // 横方向の初速
		float speedMax = 0.35f;
		float upMin = 0.18f;    // 上方向の初速
		float upMax = 0.35f;
		float gravity = 0.02f;  // 重力（0なら浮いたまま）
		int lifeMin = 18;       // 寿命（フレーム）
		int lifeMax = 30;
		float scaleMin = 0.18f; // 粒の大きさ
		float scaleMax = 0.35f;
		float spinMax = 0.3f;   // 回転の速さ
	};

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Config& config);

	// position を中心に粒をまき散らす
	void Emit(const KamataEngine::Vector3& position);

	void Update();
	void Draw();

	// 全部消す（リスタート用）
	void Clear();

private:
	struct Particle {
		bool active = false;
		KamataEngine::Vector3 velocity{};
		KamataEngine::Vector3 spin{};
		float baseScale = 0.3f;
		int life = 0;
		int maxLife = 1;
	};

	Config config_{};
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	std::vector<Particle> particles_;
	std::unique_ptr<KamataEngine::WorldTransform[]> transforms_;
};
