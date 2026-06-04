#define NOMINMAX
#include "Player.h"
#include "KamataEngine.h"
#include "WorldTransform.h"
#include <algorithm>
#include <cassert>
#include <numbers>

using namespace KamataEngine;

static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {

	// 移動入力
	// 左右移動操作
	if (onGround_) {

		if (Input::GetInstance()->PushKey(DIK_W)) {

			velocity_.y += kJumpAcceleration;
		}

		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_D)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
				}

				acceleration.x += kAcceleration;

			} else if (Input::GetInstance()->PushKey(DIK_A)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
				}

				acceleration.x -= kAcceleration;
			}

			


			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
			
			
		} else {
			velocity_.x *= (1.0f - kAttenuation);

			

		}
	} else {
	// 落下速度
		velocity_.y += -kGravityAcceleration;
	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	
	}


	// 着地フラグ
	bool landing = false;

	if (velocity_.y < 0.0f) {
		// 地面に接触しているか
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
	} else {
		if (landing) {
			worldTransform_.translation_.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}

	// 旋回処理
	float destinationRotationYTable[] = {
	    std::numbers::pi_v<float> / 2.0f,        // 右
	    std::numbers::pi_v<float> * 3.0f / 2.0f, // 左
	};

	float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

	float diff = destinationRotationY - worldTransform_.rotation_.y;


	const float pi = std::numbers::pi_v<float>;
	diff = std::fmod(diff, 2.0f * pi);
	if (diff > pi) {
		diff -= 2.0f * pi;
	} else if (diff < -pi) {
		diff += 2.0f * pi;
	}


	const float kRotationSpeed = 0.15f;


	worldTransform_.rotation_.y += diff * kRotationSpeed;

	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// 行列の計算
	UpdateWorldTransform(worldTransform_);
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }
