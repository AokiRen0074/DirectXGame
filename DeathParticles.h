#pragma once
#include "KamataEngine.h"
#include <array>
#include <numbers>

 
class DeathParticles {
public:
                           
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	// 終了フラグ
	bool isFinished_ = false;

private:
	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;

	// メンバ変数
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	// 存続時間
	static inline const float kDuration = 0.8f;
	// 移動の速さ
	static inline const float kSpeed = 0.2f;
	// 分割した1個分の角度
	static inline const float kAngleUnit = (2.0f * std::numbers::pi_v<float>) / kNumParticles;

	
	// 経過時間カウント
	float counter_ = 0.0f;

	// 色変更オブジェクト
	KamataEngine::ObjectColor objectColor_;
	// 色の数値
	KamataEngine::Vector4 color_;
};