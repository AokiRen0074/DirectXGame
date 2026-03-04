#pragma once
#include "KamataEngine.h"
#include "Player.h"

// ゲームシーン
class GameScene {
public:

	Player* player_ = nullptr;
	KamataEngine::Camera camera_; 

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();
};