#define NOMINMAX
#include "ShieldEnemy.h"
#include "GameScene.h"
#include "Player.h"
#include "WorldTransform.h"
#include <cassert>
#include <numbers>

using namespace KamataEngine;

static float EaseOut(float start, float end, float t) {
	float easeT = 1.0f - (1.0f - t) * (1.0f - t);
	return start + (end - start) * easeT;
}

static float EaseIn(float start, float end, float t) {
	float easeT = t * t;
	return start + (end - start) * easeT;
}

/*--------------------------
初期化
---------------------------*/
void ShieldEnemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
}

/*-------------------------------------
各初期化処理
--------------------------------*/
void ShieldEnemy::BehaviorRootInitialize() {}

void ShieldEnemy::BehaviorDeathInitialize() {
	deathTimer_ = 0;
	isCollisionDisabled_ = true;
}

/*-------------------------------------
のけぞり行動初期化
--------------------------------*/
void ShieldEnemy::BehaviorKnockbackInitialize() {
	knockbackTimer_ = 0;

	velocity_ = {0.0f, 0.0f, 0.0f};
}

/*-----------------------------
 ワールド座標を取得
 -------------------------------*/
KamataEngine::Vector3 ShieldEnemy::GetWorldPosition() {
	KamataEngine::Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

/*-------------------------
AABBを取得
--------------------------*/
AABB ShieldEnemy::GetAABB() {
	KamataEngine::Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

/*---------------------
衝突応答
-------------------------*/
void ShieldEnemy::OnCollision( Player* player) {
	if (behavior_ == Behavior::kDeath) {
		return;
	}

if (player->IsAttack()) {
		
		bool isPlayerRight = (player->GetLRDirection() == Player::LRDirection::kRight) && (lrDirection_ == LRDirection::kLeft);
		bool isPlayerLeft = (player->GetLRDirection() == Player::LRDirection::kLeft) && (lrDirection_ == LRDirection::kRight);

		if (isPlayerRight || isPlayerLeft) {
			if (gameScene_) {
				KamataEngine::Vector3 effectPos = worldTransform_.translation_;

			
				if (lrDirection_ == LRDirection::kLeft) {
					effectPos.x -= 0.8f;
				} else {
					effectPos.x += 0.8f;
				}

				
				effectPos.z -= 0.5f;

				
				gameScene_->CreateGuardEffect(effectPos);
			}

			player->RequestKnockback();
			behaviorRequest_ = Behavior::kKnockback;

			return;
		}
	
		behaviorRequest_ = Behavior::kDeath;

		if (gameScene_) {
			KamataEngine::Vector3 effectPos;
			effectPos.x = (worldTransform_.translation_.x + player->GetWorldTransform().translation_.x) / 2.0f;
			effectPos.y = (worldTransform_.translation_.y + player->GetWorldTransform().translation_.y) / 2.0f;
			effectPos.z = (worldTransform_.translation_.z + player->GetWorldTransform().translation_.z) / 2.0f;
			gameScene_->CreateHitEffect(effectPos);
		}
	}
}

/*-------------------------------------
通常行動
--------------------------------*/
void ShieldEnemy::BehaviorRootUpdate() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
	walkTimer_ += 1.0f / 60.0f;

	if (velocity_.x > 0.0f) {
		lrDirection_ = LRDirection::kRight;
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 右向き
	} else if (velocity_.x < 0.0f) {
		lrDirection_ = LRDirection::kLeft;
		worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f; // 左向き
	}

	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	float t = (param + 1.0f) / 2.0f;
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * t;
	worldTransform_.rotation_.x = degree * (std::numbers::pi_v<float> / 180.0f);

	UpdateWorldTransform(worldTransform_);
}

/*-------------------------------------
死亡演出行動
--------------------------------*/
void ShieldEnemy::BehaviorDeathUpdate() {
	deathTimer_++;
	const uint32_t kDeathTime = 60;

	worldTransform_.rotation_.y += 0.5f;
	worldTransform_.rotation_.x = (std::numbers::pi_v<float> / kDeathTime) * deathTimer_;
	const float kFlyUpSpeed = 0.1f;
	worldTransform_.translation_.y += kFlyUpSpeed;

	UpdateWorldTransform(worldTransform_);

	if (deathTimer_ >= kDeathTime) {
		isDead_ = true;
	}
}

/*-------------------------------------
のけぞり行動更新
--------------------------------*/
void ShieldEnemy::BehaviorKnockbackUpdate() {
	knockbackTimer_++;

	// のけぞりアニメーションの時間
	const uint32_t kBlowTime = 5;        
	const uint32_t kRecoverTime = 15;   
	const float kKnockbackAngle = 30.0f; 

	float targetAngle = (lrDirection_ == LRDirection::kRight) ? -kKnockbackAngle : kKnockbackAngle;
	// ラジアンに変換
	targetAngle = targetAngle * (std::numbers::pi_v<float> / 180.0f);

	if (knockbackTimer_ <= kBlowTime) {
		// 素早くのけぞる
		float t = static_cast<float>(knockbackTimer_) / kBlowTime;
		worldTransform_.rotation_.z = EaseOut(0.0f, targetAngle, t);

	} else {
		// ゆっくり元に戻る
		float t = static_cast<float>(knockbackTimer_ - kBlowTime) / kRecoverTime;
		worldTransform_.rotation_.z = EaseIn(targetAngle, 0.0f, t);
	}

	// 行列の更新
	UpdateWorldTransform(worldTransform_);

	// アニメーションが終わったら通常状態へ戻る
	if (knockbackTimer_ >= (kBlowTime + kRecoverTime)) {
		worldTransform_.rotation_.z = 0.0f;

		// 再び歩き出す方向と速度をセット
		velocity_ = {(lrDirection_ == LRDirection::kRight) ? kWalkSpeed : -kWalkSpeed, 0.0f, 0.0f};
		behaviorRequest_ = Behavior::kRoot;
	}
}

/*-------------------------------------
更新処理
--------------------------------*/
void ShieldEnemy::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		case Behavior::kKnockback:
			BehaviorKnockbackInitialize();
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
	case Behavior::kKnockback:
		BehaviorKnockbackUpdate();
		break;
	}
}

/*-------------------------------------
描画処理
--------------------------------*/
void ShieldEnemy::Draw() { model_->Draw(worldTransform_, *camera_); }