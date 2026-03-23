#include "TitleScene.h"
#include <cmath> 
#include "WorldTransform.h"

using namespace KamataEngine;

/*-----------------------------
デストラクタ
------------------------------*/
TitleScene::~TitleScene() {
	delete modelTitleFont_;
	delete modelPlayer_;
}

/*--------------------------------
初期化
---------------------------------*/
void TitleScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -50.0f};

	// 3Dモデルの読み込み
	modelTitleFont_ = Model::CreateFromOBJ("titleFont", true);
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	// ワールドトランスフォームの初期化
	worldTransformTitle_.Initialize();
	worldTransformTitle_.translation_ = {0.0f, 10.0f, 0.0f};
	worldTransformTitle_.scale_ = {3.0f, 3.0f, 3.0f};

	// 自キャラ
	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_ = {0.0f, -10.0f, 0.0f};
	worldTransformPlayer_.scale_ = {10.0f, 10.0f, 10.0f};
}

/*-----------------------------
更新処理
------------------------------*/
void TitleScene::Update() {

	// スペースキーで終了
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}

	// アニメーションタイマーを進める
	animationTimer_ += 0.02f;

	worldTransformTitle_.translation_.y = 10.0f + std::sin(animationTimer_) * 2.0f;

	// プレイヤーモデルをゆっくり回転させる
	worldTransformPlayer_.rotation_.y += 0.01f;
	// 少しフワフワさせる
	worldTransformPlayer_.translation_.y = -10.0f + std::cos(animationTimer_) * 1.0f;

	// 行列の更新
	UpdateWorldTransform(worldTransformTitle_);
	UpdateWorldTransform(worldTransformPlayer_);

	// カメラの行列更新
	camera_.UpdateMatrix();
}

/*------------------------------
描画処理
---------------------------------*/
void TitleScene::Draw() {
	// モデルの描画
	if (modelTitleFont_) {
		modelTitleFont_->Draw(worldTransformTitle_, camera_);
	}
	if (modelPlayer_) {
		modelPlayer_->Draw(worldTransformPlayer_, camera_);
	}
}