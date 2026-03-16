#pragma once
#include <KamataEngine.h>

class Skydome {

public:
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;

public:
	void Initialize();

	void Update();

	void Draw();
};