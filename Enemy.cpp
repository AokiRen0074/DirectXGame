#define NOMINMAX
#include "Enemy.h"
#include "WorldTransform.h"
#include <cassert>
#include <numbers>

using namespace KamataEngine;

/*--------------------------
初期化
---------------------------*/
void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	assert(model);
	model_ = model;
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 初期座標の設定
	worldTransform_.translation_ = position;

	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;

	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
}


/*-----------------------------
 ワールド座標を取得
 -------------------------------*/
KamataEngine::Vector3 Enemy::GetWorldPosition() {
	KamataEngine::Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

/*-------------------------
AABBを取得
--------------------------*/
AABB Enemy::GetAABB() {
	KamataEngine::Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

/*---------------------
衝突応答
-------------------------*/
void Enemy::OnCollision(const Player* player) {
	
	(void)player;
}


// ==========================================
// 更新処理
// ==========================================
void Enemy::Update() {

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);

	float t = (param + 1.0f) / 2.0f;
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * t;

	worldTransform_.rotation_.x = degree * (std::numbers::pi_v<float> / 180.0f);

	// ワールド行列の更新
	UpdateWorldTransform(worldTransform_);
}

// ==========================================
// 描画処理
// ==========================================
void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }