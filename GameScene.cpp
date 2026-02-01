#include "GameScene.h"

using namespace KamataEngine;

// 初期化
void GameScene::Initialize() {

	textureHandle_ = TextureManager::Load("cube/cube.jpg");
	model_=Model::Create();

	worldTransform_.Initialize();
	camera_.Initialize();
	debugCamera_ = new DebugCamera(1280, 720);


}

// 更新
void GameScene::Update() {

	debugCamera_->Update();


}

// 描画
void GameScene::Draw() {

	Model::PreDraw();
	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);
	Model::PostDraw();

}

// デストラクタ
GameScene::~GameScene() {
	delete debugCamera_;
	delete model_;
	debugCamera_ = nullptr;
}