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
	delete fade_;
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

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 2.0f);
}

/*-----------------------------
更新処理
------------------------------*/
void TitleScene::Update() {

		// メインフェーズの処理
	animationTimer_ += 0.02f;
	worldTransformTitle_.translation_.y = 10.0f + std::sin(animationTimer_) * 2.0f;
	worldTransformPlayer_.rotation_.y += 0.01f;
	worldTransformPlayer_.translation_.y = -10.0f + std::cos(animationTimer_) * 1.0f;
	UpdateWorldTransform(worldTransformTitle_);
	UpdateWorldTransform(worldTransformPlayer_);
	camera_.UpdateMatrix();

	switch (phase_) {
	case Phase::kFadeIn:
		// フェードイン中の更新
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
			fade_->Stop();
		}
		break;

	case Phase::kMain:

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case Phase::kFadeOut:
		// フェードアウト中の更新
		fade_->Update();
		// フェードアウトが終わったら、タイトルシーン自体を終了する
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
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

	if (fade_) {
		fade_->Draw();
	}
}