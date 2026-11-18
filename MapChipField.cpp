#include "MapChipField.h"
#include "WorldMatrixUtil.h"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace KamataEngine;

void MapChipField::Initialize(Camera* camera, float cellSize) {
	assert(camera);
	camera_ = camera;
	cellSize_ = cellSize;
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
			cell.worldTransform->scale_ = {cellSize_ * 0.5f, 1.0f, cellSize_ * 0.5f};
			cell.worldTransform->translation_ = GetPositionByIndex(x, z);
			UpdateWorldMatrix(*cell.worldTransform);

			cells_.push_back(std::move(cell));
		}
	}
}

void MapChipField::Draw() {
	for (Cell& cell : cells_) {
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
	return GetChipId(x, z) != 0;
}

Vector3 MapChipField::GetRandomFloorPosition(std::mt19937& rng) const {
	// 床のあるセルを集める
	std::vector<std::pair<int, int>> floors;
	for (uint32_t z = 0; z < numZ_; ++z) {
		for (uint32_t x = 0; x < numX_; ++x) {
			if (chipIds_[z][x] != 0) {
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
