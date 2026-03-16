#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>
#include "Skydome.h"
#include <memory>
#include "MapChipField.h"

// ゲームシーン
class GameScene {
public:
	Player* player_ = nullptr;
	KamataEngine::Camera camera_;
	MapChipField* mapChipField_;

	GameScene() = default; 
	~GameScene();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	
	// 3dモデル
	KamataEngine::Model* model_ = nullptr;

	// 天球3Dモデル
	KamataEngine::Model* modelSkydome_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	std::unique_ptr<Skydome> skydome_;

	public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBlocks();
};