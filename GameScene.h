#pragma once
#include "KamataEngine.h"

// ゲームシーン
class GameScene {
public:

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	private:
	uint32_t soundDataHandle_;
	    uint32_t voiceHandle_;
};