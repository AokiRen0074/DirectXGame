#define NOMINMAX
#include "Player.h"
#include "KamataEngine.h"
#include "WorldTransform.h"
#include "MapChipField.h"
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

/*-----------------------------------
初期化
---------------------------------*/
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}


/*-----------------------------------
移動処理をまとめた関数
-----------------------------------*/
void Player::InputMove() {
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


}

/*-----------------------------------
マップの衝突判定　各判定の当たり判定
-----------------------------------*/

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

//　上方向
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

if (info.moveAmount.y <= 0) {
		return;
	}


	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
		positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
	}

	bool hit = false;
	MapChipType mapChipType;
	MapChipField::IndexSet indexSet;

	
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(kLeftTop)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(kRightTop)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(kLeftTop)]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.y = (rect.bottom - worldTransform_.translation_.y) - (kHeight / 2.0f + kBlank);
		info.moveAmount.y = std::max(0.0f, info.moveAmount.y);
		info.isCeiling = true;
	}

}




void Player::CheckMapCollisionDown(CollisionMapInfo& /* info*/) {}
void Player::CheckMapCollisionRight(CollisionMapInfo& /* info*/) {}
void Player::CheckMapCollisionLeft(CollisionMapInfo& /* info*/) {}

void Player::CheckCeilingCollision(const CollisionMapInfo& info) {
	if (info.isCeiling) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0.0f;
	}
}



/*---------------------------
4角の座標
---------------------------*/
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, 
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f}, 
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f} 
	};

	
	Vector3 offset = offsetTable[static_cast<uint32_t>(corner)];

	Vector3 result;
	result.x = center.x + offset.x;
	result.y = center.y + offset.y;
	result.z = center.z + offset.z;

	return result;
}

/*-------------------------------------
更新
--------------------------------*/
void Player::Update() {

	InputMove();


	CollisionMapInfo collisionMapInfo;      
	collisionMapInfo.moveAmount = velocity_; 
	
	CheckMapCollision(collisionMapInfo);

	velocity_ = collisionMapInfo.moveAmount;

	CheckCeilingCollision(collisionMapInfo);
	
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
	// 自キャラの角度を設定する
	worldTransform_.rotation_.y = destinationRotationY;

	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// 行列の計算
	UpdateWorldTransform(worldTransform_);
}

/*--------------------
描画
--------------------*/
void Player::Draw() { 
	model_->Draw(worldTransform_, *camera_); 
}

