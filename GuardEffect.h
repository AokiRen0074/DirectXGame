#pragma once
#include "KamataEngine.h"
#include "BaseEffect.h"

class GuardEffect final : public BaseEffect{
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
	void Update()override;
	void Draw()override;

	bool IsDead() const override { return phase_ == Phase::kDead; }

private:
	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;

	KamataEngine::WorldTransform worldTransform_;

	Phase phase_ = Phase::kStop;
	uint32_t frameCounter_ = 0;
	float alpha_ = 1.0f;
};