#pragma once
#include "KamataEngine.h"
#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <vector>

// CSVからマップチップ（床）を生成・管理するクラス
//  CSVの数字 = チップID。0は何もなし、1以上はSetChipModelで登録したモデルを敷く
//  CSVの1行目が奥（+Z側）、マップ全体の中心がワールド原点
class MapChipField {
public:
	static constexpr uint32_t kMaxChipId = 8;

	// チップIDの意味（CSVの数字）
	static constexpr uint32_t kChipHole = 0;  // 何もなし（穴）
	static constexpr uint32_t kChipFloor = 1; // 床（歩ける）
	static constexpr uint32_t kChipWall = 2;  // 壁（通れない・弾も止まる）

	// 描画カリングの余白（NDC）。大きいほど画面外のマスも多めに描く
	static constexpr float kCullMargin = 0.3f;

	void Initialize(KamataEngine::Camera* camera, float cellSize = 4.0f);

	// チップIDとモデルの対応を登録（Generateより前に呼ぶ）
	void SetChipModel(uint32_t chipId, KamataEngine::Model* model);

	// そのチップを縦(Y)方向に何倍にするか（壁の高さ調整用。初期値は1）
	void SetChipHeightScale(uint32_t chipId, float scale);

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

	// その座標が歩ける床の上か（壁・穴・範囲外は false）
	bool IsFloor(const KamataEngine::Vector3& pos) const;

	// その座標が壁の中か
	bool IsWall(const KamataEngine::Vector3& pos) const;

	// from から to まで、壁にさえぎられずに撃てるか（穴は飛び越せるので無視）
	bool HasClearShot(const KamataEngine::Vector3& from, const KamataEngine::Vector3& to) const;

	// ランダムな床の上の座標（敵の出現位置などに）
	KamataEngine::Vector3 GetRandomFloorPosition(std::mt19937& rng) const;

	// ===== 経路探索（穴を避けて目標へ向かう） =====
	// 目標のいるセルからの距離マップを作る（目標のセルが変わったときだけ再計算）
	void BuildFlowField(const KamataEngine::Vector3& targetPos);

	// pos から目標へ向かう次の進行方向（XZの単位ベクトル）。道がなければ false
	bool GetFlowDirection(const KamataEngine::Vector3& pos, KamataEngine::Vector3& outDir) const;

	// from から to まで、幅を持った体で穴に落ちずにまっすぐ行けるか
	bool HasLineOfSight(const KamataEngine::Vector3& from, const KamataEngine::Vector3& to) const;

private:
	struct Cell {
		KamataEngine::Model* model = nullptr;
		std::unique_ptr<KamataEngine::WorldTransform> worldTransform;
	};

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Model* chipModels_[kMaxChipId + 1] = {};
	float chipHeightScale_[kMaxChipId + 1] = {};
	float cellSize_ = 4.0f;

	uint32_t numX_ = 0;
	uint32_t numZ_ = 0;
	std::vector<std::vector<uint32_t>> chipIds_; // [zIndex][xIndex]

	std::vector<Cell> cells_;

	// 隣のセルへ移動できるか（斜めは、両隣も床のときだけ）
	bool CanStep(int x, int z, int dx, int dz) const;

	static constexpr int kFlowUnreachable = 1 << 20;
	std::vector<int> flowDist_; // [zIndex * numX_ + xIndex] 目標までのセル数
	int flowTargetX_ = -1;
	int flowTargetZ_ = -1;
};
