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
	uint32_t textureHandle = 0;

	//3dモデル
	KamataEngine::Model* model_ = nullptr;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;

};