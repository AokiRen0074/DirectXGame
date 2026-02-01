#include "GameScene.h"

using namespace KamataEngine;

// 初期化
void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("uvChecker.png");
	sprite_ = Sprite::Create(textureHandle_, {100.0f, 50.0f});
}

// 更新
void GameScene::Update() { 
	Vector2 position = sprite_->GetPosition();

	position.x += 2.0f;
	position.y += 1.0f;

	sprite_->SetPosition(position);
}

// 描画
void GameScene::Draw() {
	Sprite::PreDraw();
	sprite_->Draw();
	Sprite::PostDraw();
}

// デストラクタ
GameScene::~GameScene() { 
	delete sprite_; 
}