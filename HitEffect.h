#pragma once
#include "KamataEngine.h"
#include <array>

class HitEffect {
public:

enum class Phase {
		kSpread,
		kFade,   
		kDead,   ）
	};
	static HitEffect* Create(const KamataEngine::Vector3& position);

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

	// 円のワールドトランスフォーム
	KamataEngine::WorldTransform circleWorldTransform_;

	// 楕円の個数
	static const uint32_t kNumEllipse = 2;

	std::array<KamataEngine::WorldTransform, kNumEllipse> ellipseWorldTransforms_;

	// 状態管理とフェード変数
	Phase phase_ = Phase::kSpread;
	uint32_t frameCounter_ = 0;
	float alpha_ = 1.0f;



};