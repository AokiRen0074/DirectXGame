#define NOMINMAX
#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

Fade::~Fade() {
	// スプライトの解放
	delete sprite_;
}

/*------------------
初期化
-----------------------*/
void Fade::Initialize() {
	// スプライトの生成
	sprite_ = Sprite::Create(0, {0.0f, 0.0f});

	sprite_->SetSize(Vector2(1280.0f, 720.0f));

	sprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
}

/*-----------------------
フェード開始関数
-------------------------------*/
void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

/*-------------------
開始、終了の関数
-------------------------*/
void Fade::Stop() { 
	status_ = Status::None;
}

bool Fade::IsFinished() const {
	return counter_ >= duration_;
}

/*-----------------------
更新処理
--------------------------*/
void Fade::Update() {
	// フェード状態による分岐
	switch (status_) {
	case Status::None:
		// 何もしない
		break;

	case Status::FadeIn:
		counter_ += 1.0f / 60.0f;
		counter_ = (std::min)(counter_, duration_);
		sprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)));
		break;

	case Status::FadeOut:
		counter_ += 1.0f / 60.0f;
		counter_ = (std::min)(counter_, duration_);
		sprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	}
}

/*----------------------
描画処理
----------------------------*/
void Fade::Draw() {

	if (status_ == Status::None) {
		return;
	}

	if (sprite_) {
		// スプライト描画前処理
		Sprite::PreDraw();

		// スプライトの描画
		sprite_->Draw();

		// スプライト描画後処理
		Sprite::PostDraw();
	}
}