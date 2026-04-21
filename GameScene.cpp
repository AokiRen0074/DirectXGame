#include "GameScene.h"

using namespace KamataEngine;

// デストラクタで全て解放
GameScene::~GameScene() {
	delete bloom_;
	// 配列の中身をすべてループで削除
	for (NeonSign* sign : neonSigns_) {
		delete sign;
	}
}

void GameScene::Initialize() {
	bloom_ = new Bloom();
	bloom_->Initialize(1280, 720);

	// 1本目：左の縦棒 (幅Xは1.0のまま！)
	NeonSign* leftBar = new NeonSign();
	leftBar->Initialize();
	leftBar->SetTransform({-1.5f, 0.0f, 0.0f}, {1.0f, 2.0f, 1.0f}, 0.0f);
	neonSigns_.push_back(leftBar);

	// 2本目：右の縦棒 (幅Xは1.0のまま！)
	NeonSign* rightBar = new NeonSign();
	rightBar->Initialize();
	rightBar->SetTransform({1.5f, 0.0f, 0.0f}, {1.0f, 2.0f, 1.0f}, 0.0f);
	neonSigns_.push_back(rightBar);

	// 3本目：真ん中の横棒
	// ★ 縦棒と同じ形を作り、Z軸で「90度（1.57ラジアン）」回転させて横に寝かせる！
	NeonSign* middleBar = new NeonSign();
	middleBar->Initialize();
	middleBar->SetTransform({0.0f, 0.0f, 0.0f}, {1.0f, 1.6f, 1.0f}, 1.57f);
	neonSigns_.push_back(middleBar);
}

void GameScene::Update() {
	// 全てのネオンを更新
	for (NeonSign* sign : neonSigns_) {
		sign->Update();

	}
}

void GameScene::Draw() {
	bloom_->PreDraw();

	// ★ HDRキャンバスに全てのネオンを描画
	for (NeonSign* sign : neonSigns_) {
		sign->Draw();
	}

	bloom_->PostDraw();

	bloom_->Execute();
	bloom_->DrawResult();
}