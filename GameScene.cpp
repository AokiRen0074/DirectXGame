#include "GameScene.h"

using namespace KamataEngine;

// 初期化
void GameScene::Initialize() { 

	camera_.Initialize();
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);



}

// 更新
void GameScene::Update() {

	camera_.UpdateMatrix();

}

// 描画
void GameScene::Draw() { 
	
	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

}