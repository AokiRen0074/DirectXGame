#pragma once
#include "KamataEngine.h"

class GuardEffect {
public:
	enum class Phase {
		kStop,   
		kSpread, 
		kFade,  
		kDead,   
	};

	static GuardEffect* Create(const KamataEngine::Vector3& position);
	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	void Initialize(const KamataEngine::Vector3& position);
	void Update();
	void Draw();

		// 調整項目を登録
	static void RegisterGlobalVariables();
	static void ApplyGlobalVariables();

	bool IsDead() const { return phase_ == Phase::kDead; }

private:
	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;

	KamataEngine::WorldTransform worldTransform_;

	Phase phase_ = Phase::kStop;
	uint32_t frameCounter_ = 0;
	float alpha_ = 1.0f;
};