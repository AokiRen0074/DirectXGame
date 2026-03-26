#pragma once
#include "KamataEngine.h"
#include "Fade.h"


class TitleScene {
public:

	enum class Phase {
		kFadeIn,
		kMain,    
		kFadeOut, 
	};

	TitleScene() = default;
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();

	// 終了フラグのゲッター
	bool IsFinished() const { return finished_; }

private:
	// カメラ
	KamataEngine::Camera camera_;

	KamataEngine::Model* modelTitleFont_ = nullptr;
	KamataEngine::WorldTransform worldTransformTitle_;

	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::WorldTransform worldTransformPlayer_;

	// アニメーション用のタイマー変数
	float animationTimer_ = 0.0f;

	// 終了フラグ
	bool finished_ = false;

	// ふぇーど
	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;
};