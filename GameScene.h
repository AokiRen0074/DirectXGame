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

	// デストラクタ
	~GameScene();
	private:
	uint32_t textureHandle_ = 0;
	 KamataEngine::Sprite* sprite_ = nullptr;
};