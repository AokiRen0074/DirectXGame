#include <KamataEngine.h>
#include "MapChipField.h"
#include <fstream>
#include <sstream>
#include <cassert>

using namespace KamataEngine;

// マップチップデータをリセット
void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

// マップチップの読み込み
void MapChipField::LoadMapChipCsv(const std::string& filepath) {
	ResetMapChipData(); 

	// ファイルを開く
	std::ifstream file;
	file.open(filepath);
	assert(file.is_open());

	// マップチップCSV
	std::stringstream mapChipCsv;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	file.close();

	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		std::string line;
		std::getline(mapChipCsv, line);

		// 一行分の文字列をストリームに変換して解析しやすくなる
		std::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			std::string word;
			std::getline(lineStream, word, ',');
			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}

}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {

	// X座標が範囲外なら空白を返す
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	// Y座標が範囲外なら空白を返す
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

	// 範囲内なら、その座標のマップチップのデータを返す
	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);

}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) {
	MapChipField::IndexSet indexSet = {};


	indexSet.xIndex = static_cast<uint32_t>((position.x + MapChipField::kBlockWidth / 2.0f) / MapChipField::kBlockWidth);

	uint32_t tempYIndex = static_cast<uint32_t>((position.y + MapChipField::kBlockHeight / 2.0f) / MapChipField::kBlockHeight);
	indexSet.yIndex = MapChipField::kNumBlockVirtical - 1 - tempYIndex;

	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {

	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	MapChipField::Rect rect;

	rect.left = center.x - MapChipField::kBlockWidth / 2.0f;
	rect.right = center.x + MapChipField::kBlockWidth / 2.0f; 
	rect.bottom = center.y - MapChipField::kBlockHeight / 2.0f;
	rect.top = center.y + MapChipField::kBlockHeight / 2.0f;

	return rect;


}