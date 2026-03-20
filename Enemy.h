#pragma once
#include "KamataEngine.h"

class Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	static inline const float kWalkSpeed = 0.03f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	/*------------------
	アニメーション
	-------------------*/

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 8.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = -8.0f;
	// アニメーションの周期となる時間[秒]（例：1秒で1ループ）
	static inline const float kWalkMotionTime = 1.0f;

	// タイマー（経過時間 tn を記録する変数）
	float walkTimer_ = 0.0f;
};