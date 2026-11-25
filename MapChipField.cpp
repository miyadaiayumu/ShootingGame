#include "MapChipField.h"
#include "ScreenUtil.h"
#include "WorldMatrixUtil.h"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <queue>
#include <sstream>

using namespace KamataEngine;

void MapChipField::Initialize(Camera* camera, float cellSize) {
	assert(camera);
	camera_ = camera;
	cellSize_ = cellSize;
	for (float& h : chipHeightScale_) {
		h = 1.0f;
	}
}

void MapChipField::SetChipHeightScale(uint32_t chipId, float scale) {
	assert(chipId >= 1 && chipId <= kMaxChipId);
	chipHeightScale_[chipId] = scale;
}

void MapChipField::SetChipModel(uint32_t chipId, Model* model) {
	assert(chipId >= 1 && chipId <= kMaxChipId);
	chipModels_[chipId] = model;
}

bool MapChipField::LoadCSV(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return false;
	}

	chipIds_.clear();
	numX_ = 0;
	flowDist_.clear();
	flowTargetX_ = -1;
	flowTargetZ_ = -1;

	std::string line;
	while (std::getline(file, line)) {
		// 改行コード(\r)・空行対策
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		if (line.empty()) {
			continue;
		}

		std::vector<uint32_t> row;
		std::stringstream ss(line);
		std::string cell;
		while (std::getline(ss, cell, ',')) {
			// 空欄は0（何もなし）扱い
			uint32_t id = cell.empty() ? 0 : static_cast<uint32_t>(std::strtoul(cell.c_str(), nullptr, 10));
			if (id > kMaxChipId) {
				id = 0;
			}
			row.push_back(id);
		}
		numX_ = (std::max)(numX_, static_cast<uint32_t>(row.size()));
		chipIds_.push_back(std::move(row));
	}

	numZ_ = static_cast<uint32_t>(chipIds_.size());

	// 行の長さが揃っていなくても安全なように0で埋める
	for (auto& row : chipIds_) {
		row.resize(numX_, 0);
	}

	return numX_ > 0 && numZ_ > 0;
}

void MapChipField::Generate() {
	cells_.clear();

	for (uint32_t z = 0; z < numZ_; ++z) {
		for (uint32_t x = 0; x < numX_; ++x) {
			uint32_t id = chipIds_[z][x];
			if (id == 0 || !chipModels_[id]) {
				continue;
			}

			Cell cell;
			cell.model = chipModels_[id];
			cell.worldTransform = std::make_unique<WorldTransform>();
			cell.worldTransform->Initialize();
			// モデルは2x2の平面なので、cellSize / 2 倍で1マスにぴったり
			cell.worldTransform->scale_ = {cellSize_ * 0.5f, chipHeightScale_[id], cellSize_ * 0.5f};
			cell.worldTransform->translation_ = GetPositionByIndex(x, z);
			UpdateWorldMatrix(*cell.worldTransform);

			cells_.push_back(std::move(cell));
		}
	}
}

void MapChipField::Draw() {
	for (Cell& cell : cells_) {
		// 画面内のマスだけ描画（マスの中心で判定するので、大きめの余白を付ける）
		if (!IsInScreen(cell.worldTransform->translation_, *camera_, kCullMargin)) {
			continue;
		}
		cell.model->Draw(*cell.worldTransform, *camera_);
	}
}

uint32_t MapChipField::GetChipId(int xIndex, int zIndex) const {
	if (xIndex < 0 || zIndex < 0 || xIndex >= static_cast<int>(numX_) || zIndex >= static_cast<int>(numZ_)) {
		return 0;
	}
	return chipIds_[zIndex][xIndex];
}

Vector3 MapChipField::GetPositionByIndex(int xIndex, int zIndex) const {
	return {
	    (xIndex + 0.5f - numX_ * 0.5f) * cellSize_,
	    0.0f,
	    (numZ_ * 0.5f - zIndex - 0.5f) * cellSize_,
	};
}

bool MapChipField::GetIndexByPosition(const Vector3& pos, int& xIndex, int& zIndex) const {
	xIndex = static_cast<int>(std::floor(pos.x / cellSize_ + numX_ * 0.5f));
	zIndex = static_cast<int>(std::floor(numZ_ * 0.5f - pos.z / cellSize_));

	return xIndex >= 0 && zIndex >= 0 && xIndex < static_cast<int>(numX_) && zIndex < static_cast<int>(numZ_);
}

bool MapChipField::IsFloor(const Vector3& pos) const {
	int x, z;
	if (!GetIndexByPosition(pos, x, z)) {
		return false;
	}
	return GetChipId(x, z) == kChipFloor;
}

bool MapChipField::IsWall(const Vector3& pos) const {
	int x, z;
	if (!GetIndexByPosition(pos, x, z)) {
		return false;
	}
	return GetChipId(x, z) == kChipWall;
}

bool MapChipField::HasClearShot(const Vector3& from, const Vector3& to) const {
	const float dx = to.x - from.x;
	const float dz = to.z - from.z;
	const float dist = std::sqrt(dx * dx + dz * dz);
	const float kStep = 0.5f; // 壁(1マス=cellSize)より十分細かく調べる

	const int steps = static_cast<int>(std::ceil(dist / kStep));
	for (int i = 1; i < steps; ++i) {
		float t = static_cast<float>(i) / static_cast<float>(steps);
		Vector3 p = {from.x + dx * t, 0.0f, from.z + dz * t};
		if (IsWall(p)) {
			return false;
		}
	}
	return true;
}

Vector3 MapChipField::GetRandomFloorPosition(std::mt19937& rng) const {
	// 床のあるセルを集める
	std::vector<std::pair<int, int>> floors;
	for (uint32_t z = 0; z < numZ_; ++z) {
		for (uint32_t x = 0; x < numX_; ++x) {
			if (chipIds_[z][x] == kChipFloor) {
				floors.emplace_back(x, z);
			}
		}
	}
	if (floors.empty()) {
		return {0.0f, 0.0f, 0.0f};
	}

	std::uniform_int_distribution<size_t> pick(0, floors.size() - 1);
	auto [xi, zi] = floors[pick(rng)];

	// セル内でも少しばらつかせる
	float half = cellSize_ * 0.4f;
	std::uniform_real_distribution<float> offset(-half, half);

	Vector3 pos = GetPositionByIndex(xi, zi);
	pos.x += offset(rng);
	pos.z += offset(rng);
	return pos;
}

// ===================== 経路探索 =====================

bool MapChipField::CanStep(int x, int z, int dx, int dz) const {
	// 行き先が歩ける床であること（壁・穴は不可）
	if (GetChipId(x + dx, z + dz) != kChipFloor) {
		return false;
	}
	// 斜め移動は、角をかすめて壁や穴に当たらないよう両隣も床のときだけ
	if (dx != 0 && dz != 0) {
		if (GetChipId(x + dx, z) != kChipFloor || GetChipId(x, z + dz) != kChipFloor) {
			return false;
		}
	}
	return true;
}

void MapChipField::BuildFlowField(const Vector3& targetPos) {
	int tx, tz;
	if (!GetIndexByPosition(targetPos, tx, tz) || GetChipId(tx, tz) != kChipFloor) {
		return; // 目標が床の上でなければ、前回の結果をそのまま使う
	}
	// 目標のセルが前回と同じなら再計算しない
	if (tx == flowTargetX_ && tz == flowTargetZ_ && !flowDist_.empty()) {
		return;
	}
	flowTargetX_ = tx;
	flowTargetZ_ = tz;

	// 幅優先探索：目標から広げて、各セルまでの歩数を記録する
	flowDist_.assign(static_cast<size_t>(numX_) * numZ_, kFlowUnreachable);
	std::queue<std::pair<int, int>> queue;
	flowDist_[tz * numX_ + tx] = 0;
	queue.push({tx, tz});

	while (!queue.empty()) {
		auto [x, z] = queue.front();
		queue.pop();
		int dist = flowDist_[z * numX_ + x];

		for (int dz = -1; dz <= 1; ++dz) {
			for (int dx = -1; dx <= 1; ++dx) {
				if (dx == 0 && dz == 0) {
					continue;
				}
				if (!CanStep(x, z, dx, dz)) {
					continue;
				}
				int& next = flowDist_[(z + dz) * numX_ + (x + dx)];
				if (next != kFlowUnreachable) {
					continue;
				}
				next = dist + 1;
				queue.push({x + dx, z + dz});
			}
		}
	}
}

bool MapChipField::GetFlowDirection(const Vector3& pos, Vector3& outDir) const {
	int x, z;
	if (flowDist_.empty() || !GetIndexByPosition(pos, x, z)) {
		return false;
	}

	int current = flowDist_[z * numX_ + x];
	if (current == kFlowUnreachable || current == 0) {
		return false; // 道がない、または目標のセルにいる
	}

	// 目標にいちばん近い（歩数の小さい）隣のセルを探す
	int best = current;
	int bestX = -1, bestZ = -1;
	for (int dz = -1; dz <= 1; ++dz) {
		for (int dx = -1; dx <= 1; ++dx) {
			if ((dx == 0 && dz == 0) || !CanStep(x, z, dx, dz)) {
				continue;
			}
			int d = flowDist_[(z + dz) * numX_ + (x + dx)];
			if (d < best) {
				best = d;
				bestX = x + dx;
				bestZ = z + dz;
			}
		}
	}
	if (bestX < 0) {
		return false;
	}

	// そのセルの中心へ向かう
	Vector3 center = GetPositionByIndex(bestX, bestZ);
	float vx = center.x - pos.x;
	float vz = center.z - pos.z;
	float len = std::sqrt(vx * vx + vz * vz);
	if (len < 0.001f) {
		return false;
	}
	outDir = {vx / len, 0.0f, vz / len};
	return true;
}

bool MapChipField::HasLineOfSight(const Vector3& from, const Vector3& to) const {
	const float dx = to.x - from.x;
	const float dz = to.z - from.z;
	const float dist = std::sqrt(dx * dx + dz * dz);
	if (dist < 0.001f) {
		return true;
	}

	const float ux = dx / dist, uz = dz / dist; // 進行方向
	const float px = -uz, pz = ux;              // 横方向
	const float kStep = 1.0f;                   // 確認する間隔
	const float kHalfWidth = 0.9f;              // 体の半分の幅
	const float offsets[3] = {-kHalfWidth, 0.0f, kHalfWidth};

	const int steps = static_cast<int>(std::ceil(dist / kStep));
	for (int i = 0; i <= steps; ++i) {
		float t = (std::min)(i * kStep, dist);
		for (float off : offsets) {
			Vector3 p = {from.x + ux * t + px * off, 0.0f, from.z + uz * t + pz * off};
			if (!IsFloor(p)) {
				return false;
			}
		}
	}
	return true;
}
