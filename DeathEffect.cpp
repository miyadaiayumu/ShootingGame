#include "DeathEffect.h"
#include "WorldMatrixUtil.h"
#include <assert.h>
#include <cmath>
#include <random>

using namespace KamataEngine;

void DeathEffect::Initialize(Model* model, Camera* camera, const Config& config) {
	assert(model);
	assert(camera);
	model_ = model;
	camera_ = camera;
	config_ = config;

	particles_.assign(config_.poolSize, Particle{});
	transforms_ = std::make_unique<WorldTransform[]>(config_.poolSize);
	for (int i = 0; i < config_.poolSize; ++i) {
		transforms_[i].Initialize();
	}
}

void DeathEffect::Emit(const Vector3& position) {
	static std::mt19937 rng{std::random_device{}()};
	std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
	std::uniform_real_distribution<float> speedDist(config_.speedMin, config_.speedMax);
	std::uniform_real_distribution<float> upDist(config_.upMin, config_.upMax);
	std::uniform_int_distribution<int> lifeDist(config_.lifeMin, config_.lifeMax);
	std::uniform_real_distribution<float> scaleDist(config_.scaleMin, config_.scaleMax);
	std::uniform_real_distribution<float> spinDist(-config_.spinMax, config_.spinMax);

	int emitted = 0;
	for (int i = 0; i < config_.poolSize && emitted < config_.burstCount; ++i) {
		Particle& p = particles_[i];
		if (p.active) {
			continue;
		}

		float angle = angleDist(rng);
		float speed = speedDist(rng);
		p.active = true;
		p.velocity = {std::cos(angle) * speed, upDist(rng), std::sin(angle) * speed};
		p.spin = {spinDist(rng), spinDist(rng), spinDist(rng)};
		p.baseScale = scaleDist(rng);
		p.maxLife = lifeDist(rng);
		p.life = p.maxLife;

		WorldTransform& wt = transforms_[i];
		wt.translation_ = position;
		wt.rotation_ = {0.0f, 0.0f, 0.0f};
		wt.scale_ = {p.baseScale, p.baseScale, p.baseScale};
		UpdateWorldMatrix(wt);

		++emitted;
	}
}

void DeathEffect::Update() {
	for (int i = 0; i < config_.poolSize; ++i) {
		Particle& p = particles_[i];
		if (!p.active) {
			continue;
		}

		WorldTransform& wt = transforms_[i];

		// 重力で落ちて、床で小さく跳ねる
		p.velocity.y -= config_.gravity;
		wt.translation_.x += p.velocity.x;
		wt.translation_.y += p.velocity.y;
		wt.translation_.z += p.velocity.z;
		if (wt.translation_.y < 0.1f) {
			wt.translation_.y = 0.1f;
			p.velocity.y = -p.velocity.y * 0.4f;
			p.velocity.x *= 0.85f;
			p.velocity.z *= 0.85f;
		}

		// 回転しながら、寿命に合わせて小さくなって消える
		wt.rotation_.x += p.spin.x;
		wt.rotation_.y += p.spin.y;
		wt.rotation_.z += p.spin.z;

		if (--p.life <= 0) {
			p.active = false;
			continue;
		}
		float s = p.baseScale * static_cast<float>(p.life) / static_cast<float>(p.maxLife);
		wt.scale_ = {s, s, s};

		UpdateWorldMatrix(wt);
	}
}

void DeathEffect::Draw() {
	for (int i = 0; i < config_.poolSize; ++i) {
		if (particles_[i].active) {
			model_->Draw(transforms_[i], *camera_);
		}
	}
}

void DeathEffect::Clear() {
	for (Particle& p : particles_) {
		p.active = false;
	}
}
