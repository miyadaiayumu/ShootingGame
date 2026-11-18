#pragma once
#include "KamataEngine.h"
#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <vector>

class MapChipField {
public:
	static constexpr uint32_t kMaxChipId = 8;

	void Initialize(KamataEngine::Camera* camera, float cellSize = 4.0f);

	// チップIDとモデルの対応を登録（Generateより前に呼ぶ）
	void SetChipModel(uint32_t chipId, KamataEngine::Model* model);

	// CSV読み込み。失敗したらfalse
	bool LoadCSV(const std::string& filePath);

	// 読み込んだデータからWorldTransformを生成
	void Generate();

	void Draw();

	uint32_t GetNumX() const { return numX_; }
	uint32_t GetNumZ() const { return numZ_; }
	float GetCellSize() const { return cellSize_; }

	// 範囲外は0を返す
	uint32_t GetChipId(int xIndex, int zIndex) const;

	// セル中心のワールド座標（y=0）
	KamataEngine::Vector3 GetPositionByIndex(int xIndex, int zIndex) const;

	// ワールド座標 → セル番号。マップの範囲外ならfalse
	bool GetIndexByPosition(const KamataEngine::Vector3& pos, int& xIndex, int& zIndex) const;

	// その座標が床の上か（チップID != 0）
	bool IsFloor(const KamataEngine::Vector3& pos) const;

	// ランダムな床の上の座標（敵の出現位置などに）
	KamataEngine::Vector3 GetRandomFloorPosition(std::mt19937& rng) const;

private:
	struct Cell {
		KamataEngine::Model* model = nullptr;
		std::unique_ptr<KamataEngine::WorldTransform> worldTransform;
	};

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Model* chipModels_[kMaxChipId + 1] = {};
	float cellSize_ = 4.0f;

	uint32_t numX_ = 0;
	uint32_t numZ_ = 0;
	std::vector<std::vector<uint32_t>> chipIds_; // [zIndex][xIndex]

	std::vector<Cell> cells_;
};
