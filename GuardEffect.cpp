#include "GuardEffect.h"
#include <cassert>
#include "WorldTransform.h"

using namespace KamataEngine;

KamataEngine::Model* GuardEffect::model_ = nullptr;
KamataEngine::Camera* GuardEffect::camera_ = nullptr;

static float EaseOut(float start, float end, float t) {
	float easeT = 1.0f - (1.0f - t) * (1.0f - t);
	return start + (end - start) * easeT;
}

void GuardEffect::RegisterGlobalVariables() {}

void GuardEffect::ApplyGlobalVariables() {}


GuardEffect* GuardEffect::Create(const KamataEngine::Vector3& position) {
	GuardEffect* instance = new GuardEffect();
	assert(instance);
	instance->Initialize(position);
	return instance;
}

void GuardEffect::Initialize(const KamataEngine::Vector3& position) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	phase_ = Phase::kStop; 
	frameCounter_ = 0;
	alpha_ = 1.0f;
}

void GuardEffect::Update() {
	const uint32_t kStopTime = 5;  
	const uint32_t kSpreadTime = 8;
	const uint32_t kFadeTime = 12; 

	switch (phase_) {
	case Phase::kStop:
		frameCounter_++;
		if (frameCounter_ >= kStopTime) {
			phase_ = Phase::kSpread;
			frameCounter_ = 0;
		}
		break;

	case Phase::kSpread:
		frameCounter_++;
		{
			float t = static_cast<float>(frameCounter_) / kSpreadTime;
		
			worldTransform_.scale_ = {EaseOut(0.5f, 2.5f, t), EaseOut(0.5f, 2.5f, t), EaseOut(0.5f, 2.5f, t)};
		}
		if (frameCounter_ >= kSpreadTime) {
			phase_ = Phase::kFade; 
			frameCounter_ = 0;
		}
		break;

	case Phase::kFade:
		frameCounter_++;
		{
			float t = static_cast<float>(frameCounter_) / kFadeTime;
			alpha_ = EaseOut(1.0f, 0.0f, t); // 透明にしていく
		}
		if (frameCounter_ >= kFadeTime) {
			phase_ = Phase::kDead; // 消滅
		}
		break;

	case Phase::kDead:
		break;
	}

	UpdateWorldTransform(worldTransform_);
}

void GuardEffect::Draw() {
	if (phase_ == Phase::kDead)
		return;

	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}