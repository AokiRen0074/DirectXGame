#include "GameScene.h"

using namespace KamataEngine;

// 初期化
void GameScene::Initialize() {
	// 描画
	textureHandle = TextureManager::Load("cube/cube.jpg");
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラの初期化
	camera_.Initialize();

}

// 更新
void GameScene::Update() {}

// 描画
void GameScene::Draw() {

	Model::PreDraw();
	model_->Draw(worldTransform_, camera_, textureHandle);
	Model::PostDraw();



}

// デストラクタ
GameScene::~GameScene() {

	delete model_;
}