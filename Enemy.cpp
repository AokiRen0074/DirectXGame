#define NOMINMAX
#include "Enemy.h"
#include "WorldTransform.h"
#include <cassert>
#include <numbers>
#include <numbers>
#include "Player.h"
#include "GameScene.h"

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

void Enemy::RegisterGlobalVariables() {}

void Enemy::ApplyGlobalVariables() {}

/*-------------------------------------
各初期化処理
--------------------------------*/
void Enemy::BehaviorRootInitialize() {}

void Enemy::BehaviorDeathInitialize() {
	deathTimer_ = 0; 
	isCollisionDisabled_ = true;
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

	if (behavior_ == Behavior::kDeath) {
		// デス演出中なら何もしない
		return;
	}

	// プレイヤーが攻撃中ならデス演出に移行
	if (player->IsAttack()) {
		behaviorRequest_ = Behavior::kDeath;
	}

	if (gameScene_) {
		KamataEngine::Vector3 effectPos;
		// 中間地点
		effectPos.x = (worldTransform_.translation_.x + player->GetWorldTransform().translation_.x) / 2.0f;
		effectPos.y = (worldTransform_.translation_.y + player->GetWorldTransform().translation_.y) / 2.0f;
		effectPos.z = (worldTransform_.translation_.z + player->GetWorldTransform().translation_.z) / 2.0f;

		gameScene_->CreateHitEffect(effectPos);
	}

}


/*-------------------------------------
通常行動
--------------------------------*/
void Enemy::BehaviorRootUpdate() {
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

/*-------------------------------------
死亡演出行動
--------------------------------*/
void Enemy::BehaviorDeathUpdate() {
	// やられアニメーション
	deathTimer_++;
	const uint32_t kDeathTime = 60;

	worldTransform_.rotation_.y += 0.5f;
	worldTransform_.rotation_.x = (std::numbers::pi_v<float> / kDeathTime) * deathTimer_;
	const float kFlyUpSpeed = 0.1f;
	worldTransform_.translation_.y += kFlyUpSpeed;
	// 行列の更新
	UpdateWorldTransform(worldTransform_);

	if (deathTimer_ >= kDeathTime) {
		isDead_ = true;
	}
}

/*---------------------------------------
更新処理
---------------------------*/
void Enemy::Update() {

if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		default:
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}
}

/*----------------------------
描画処理
--------------------------------*/
void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }