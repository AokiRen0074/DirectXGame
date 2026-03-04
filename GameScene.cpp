#include "GameScene.h"

using namespace KamataEngine;

// 初期化
void GameScene::Initialize() {
	Model* model = Model::CreateFromOBJ("cube", true);
	uint32_t textureHandle = TextureManager::Load("uvChecker.png");
	camera_.Initialize();
	camera_.translation_ = {0, 0, -10.0f};
	player_ = new Player();

	player_->Initialize(model, textureHandle, &camera_);
}

// 更新
void GameScene::Update() {
	camera_.UpdateMatrix();

	if (player_ != nullptr) {
		player_->Update();
	}
}

// 描画
void GameScene::Draw() {
	if (player_ != nullptr) {
		player_->Draw();
	}
}