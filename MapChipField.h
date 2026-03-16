#pragma once
#include <KamataEngine.h>
#include <map>

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

// マップチップのデータ
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};

}

class MapChipField {
public:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 12;
	static inline const uint32_t kNumBlockHorizontal = 20;

	MapChipData mapChipData_;
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	public:
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filepath);
	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

};