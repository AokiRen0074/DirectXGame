#include "HitEffect.h"
#include <cassert>
#include "WorldTransform.h"
#include <random> 
#include <numbers>


static float EaseOut(float start, float end, float t) {
	float easeT = 1.0f - (1.0f - t) * (1.0f - t);
	return start + (end - start) * easeT;
}

// 静的メンバ変数の実体
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(const KamataEngine::Vector3& position) {
	// インスタンス生成
	HitEffect* instance = new HitEffect();
	// newの失敗を検出
	assert(instance);
	// インスタンスの初期化
	instance->Initialize(position);
	// 初期化したインスタンスを返す
	return instance;
}

/*-------------------------
初期化
---------------------------------*/
void HitEffect::Initialize(const KamataEngine::Vector3& position) {
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;

	phase_ = Phase::kSpread; 
	frameCounter_ = 0;
	alpha_ = 1.0f;

	//　乱数エンジンの初期化
	std::random_device seedGenerator;
	std::mt19937_64 randomEngine(seedGenerator());

	// 指定範囲の乱数生成
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	// 楕円エフェクトの初期化
	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.Initialize();

		// 楕円のスケール
		worldTransform.scale_ = {0.1f, 1.5f, 1.0f};
		// Z軸まわりの回転に乱数を入れる
		worldTransform.rotation_ = {0.0f, 0.0f, rotationDistribution(randomEngine)};

		worldTransform.translation_ = position;
	}

	frameCounter_ = 0;
}

void HitEffect::RegisterGlobalVariables() {}

void HitEffect::ApplyGlobalVariables() {}

/*-----------------------
更新
--------------------------*/
void HitEffect::Update() {
	const uint32_t kSpreadTime = 10; 
	const uint32_t kFadeTime = 15;

	switch (phase_) {
	case Phase::kSpread: {
		frameCounter_++;
		float t = static_cast<float>(frameCounter_) / kSpreadTime;

		// 円の拡大
		circleWorldTransform_.scale_ = {EaseOut(0.1f, 1.5f, t), EaseOut(0.1f, 1.5f, t), EaseOut(0.1f, 1.5f, t)};

		// 楕円の拡大
		for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransforms_) {
			worldTransform.scale_.y = EaseOut(1.5f, 3.0f, t);
		}

		if (frameCounter_ >= kSpreadTime) {
			phase_ = Phase::kFade; 
			frameCounter_ = 0;     
		}
		break;
	}
	case Phase::kFade: {
		frameCounter_++;
		float t = static_cast<float>(frameCounter_) / kFadeTime;

		alpha_ = EaseOut(1.0f, 0.0f, t);

		if (frameCounter_ >= kFadeTime) {
			phase_ = Phase::kDead; 
		}
		break;
	}
	case Phase::kDead:
		break;
	}

	UpdateWorldTransform(circleWorldTransform_);

	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransforms_) {
		UpdateWorldTransform(worldTransform);
	}
}

/*-----------------------
描画
------------------------------*/
void HitEffect::Draw() {
	if (phase_ == Phase::kDead)
		return; 

	if (model_) {
		model_->Draw(circleWorldTransform_, *camera_);


		for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransforms_) {
			model_->Draw(worldTransform, *camera_);
		}
	}
}