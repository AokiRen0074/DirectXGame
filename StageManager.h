#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cassert> 

struct StageData {
	std::string name;  // ステージ名
	int32_t timeLimit; // 制限時間
};


class StageManager {
private:
	// 全ステージデータ
	std::vector<StageData> stageDatas_;
	int32_t currentStageIndex_ = 0;

public:
	void LoadStageDatas();

	const StageData& GetStageData(int32_t index) const {
		
		assert(index >= 0 && index < stageDatas_.size());
		return stageDatas_[index];
	};

	void SetCurrentStageIndex(int32_t index) {

		assert(index >= 0 && index < stageDatas_.size());
		currentStageIndex_ = index;
	}

	void SetCurrentStageIndexByName(const std::string& name);


	/// 現在のステージ番号の取得
	int32_t GetCurrentStageIndex() const { return currentStageIndex_; }

	/// 現在ステージのステージデータ取得
	const StageData& GetCurrentStageData() const {
		return GetStageData(currentStageIndex_);
	}
};