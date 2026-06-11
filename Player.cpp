#define NOMINMAX
#include "Player.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "WorldTransform.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include "GlobalVariables.h"

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

// イージング
static float EaseOut(float start, float end, float t) {
	float easeT = 1.0f - (1.0f - t) * (1.0f - t);
	return start + (end - start) * easeT;
}

static float EaseIn(float start, float end, float t) {
	float easeT = t * t;
	return start + (end - start) * easeT;
}



/*-----------------------------------
初期化
---------------------------------*/
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	modelAttack_ = modelAttack;
	worldTransformAttack_.Initialize();

	// デバッガによる確認
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	globalVariables->AddItem(groupName, "Test", 90);

	globalVariables->AddItem(groupName, "PlayerAcceleration", kAcceleration);
	globalVariables->AddItem(groupName, "PlayerJumpAcceleration", kJumpAcceleration);

	ApplyGlobalVariables();

}

/*---------------------------
項目調整の適応
---------------------------*/


void Player::RegisterGlobalVariables() {}

void Player::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	kAcceleration = globalVariables->GetFloatValue(groupName, "PlayerAcceleration");
	kJumpAcceleration = globalVariables->GetFloatValue(groupName, "PlayerJumpAcceleration");

}


/*-------------------------------------
通常行動初期化
--------------------------------*/
void Player::BehaviorRootInitialize() {}

/*-------------------------------------
攻撃行動初期化
--------------------------------*/
void Player::BehaviorAttackInitialize() {
	// カウンター初期化
	attackParameter_ = 0;
	attackPhase_ = AttackPhase::kCharge;
	velocity_ = {0.0f, 0.0f, 0.0f};
}

/*-------------------------------------
ノックバック行動初期化
--------------------------------*/
void Player::BehaviorKnockbackInitialize() {
	knockbackPhase_ = KnockbackPhase::kBlow;
	knockbackTimer_ = 0;

	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	
	float blowSpeedX = (lrDirection_ == LRDirection::kRight) ? -0.8f : 0.8f;

	velocity_ = {blowSpeedX, 0.4f, 0.0f};
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
	// 判定する4つの方向を定義
	enum class Direction { kUp, kDown, kRight, kLeft };
	Direction directions[] = {Direction::kUp, Direction::kDown, Direction::kRight, Direction::kLeft};

	// 4方向分、順番に判定を回す
	for (Direction dir : directions) {

		// 移動量チェック
		if (dir == Direction::kUp && info.moveAmount.y <= 0)
			continue;
		if (dir == Direction::kDown && info.moveAmount.y >= 0)
			continue;
		if (dir == Direction::kRight && info.moveAmount.x <= 0)
			continue;
		if (dir == Direction::kLeft && info.moveAmount.x >= 0)
			continue;

		// 移動後の4つの角の座標を計算
		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
			positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
		}

		Corner corner1 = kLeftTop, corner2 = kLeftTop;
		int32_t xOffset = 0, yOffset = 0;

		switch (dir) {
		case Direction::kUp:
			corner1 = kLeftTop;
			corner2 = kRightTop;
			yOffset = 1;
			break;
		case Direction::kDown:
			corner1 = kLeftBottom;
			corner2 = kRightBottom;
			yOffset = -1;
			break;
		case Direction::kRight:
			corner1 = kRightTop;
			corner2 = kRightBottom;
			xOffset = -1;
			break;
		case Direction::kLeft:
			corner1 = kLeftTop;
			corner2 = kLeftBottom;
			xOffset = 1;
			break;
		}

		// 当たり判定
		bool hit = false;

		// 1つ目の角
		MapChipField::IndexSet indexSet1 = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(corner1)]);
		MapChipType type1 = mapChipField_->GetMapChipTypeByIndex(indexSet1.xIndex, indexSet1.yIndex);
		MapChipType typeNext1 = mapChipField_->GetMapChipTypeByIndex(indexSet1.xIndex + xOffset, indexSet1.yIndex + yOffset);
		if (type1 == MapChipType::kBlock && typeNext1 != MapChipType::kBlock)
			hit = true;

		// 2つ目の角
		MapChipField::IndexSet indexSet2 = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(corner2)]);
		MapChipType type2 = mapChipField_->GetMapChipTypeByIndex(indexSet2.xIndex, indexSet2.yIndex);
		MapChipType typeNext2 = mapChipField_->GetMapChipTypeByIndex(indexSet2.xIndex + xOffset, indexSet2.yIndex + yOffset);
		if (type2 == MapChipType::kBlock && typeNext2 != MapChipType::kBlock)
			hit = true;

		// ブロックにヒットした時の押し戻し処理
		if (hit) {
			// 現在の代表角の座標を計算
			Vector3 currentPosCorner = CornerPosition(worldTransform_.translation_, corner1);
			MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(currentPosCorner);

			// 移動前と移動後でセル境界をまたいだかを判定
			bool isIntersect = false;
			if (dir == Direction::kUp || dir == Direction::kDown) {
				// 上下の移動時はYの番号が変わったかをチェック
				if (indexSetNow.yIndex != indexSet1.yIndex) {
					isIntersect = true;
				}
			} else { // kRight, kLeft
				// 左右の移動時はXの番号が変わったかをチェック
				if (indexSetNow.xIndex != indexSet1.xIndex) {
					isIntersect = true;
				}
			}

			// セル境界をしっかりまたいでいた場合のみ、押し戻し処理を行う
			if (isIntersect) {
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet1.xIndex, indexSet1.yIndex);

				switch (dir) {
				case Direction::kUp:
					info.moveAmount.y = (rect.bottom - worldTransform_.translation_.y) - (kHeight / 2.0f + kBlank);
					info.moveAmount.y = std::max(0.0f, info.moveAmount.y);
					info.isCeiling = true;
					break;
				case Direction::kDown:
					info.moveAmount.y = (rect.top - worldTransform_.translation_.y) + (kHeight / 2.0f + kBlank);
					info.moveAmount.y = std::min(0.0f, info.moveAmount.y);
					info.isGround = true;
					break;
				case Direction::kRight:
					info.moveAmount.x = (rect.left - worldTransform_.translation_.x) - (kWidth / 2.0f + kBlank);
					info.moveAmount.x = std::max(0.0f, info.moveAmount.x);
					info.isWall = true;
					break;
				case Direction::kLeft:
					info.moveAmount.x = (rect.right - worldTransform_.translation_.x) + (kWidth / 2.0f + kBlank);
					info.moveAmount.x = std::min(0.0f, info.moveAmount.x);
					info.isWall = true;
					break;
				}
			}
		}
	}
}

/*---------------------------------
壁に接触しているときの判定
------------------------------------*/
void Player::CheckWallCollision(const CollisionMapInfo& info) {
	// 壁接触による減速
	if (info.isWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}
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

/*--------------------------
接地状態の切り替え処理
--------------------------------*/
void Player::SwitchOnGround(const CollisionMapInfo& info) {
	// 自キャラが接地状態？
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {

			// 落下判定（足元に床があるかチェック！）

			Vector3 currentPos = worldTransform_.translation_;

			bool hit = false;
			MapChipType mapChipType;
			MapChipField::IndexSet indexSet;

			// 左下点の判定
			Vector3 leftBottomPos = CornerPosition(currentPos, kLeftBottom);
			leftBottomPos.y -= kGroundSearchHeight; // 少し下に延長

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(leftBottomPos);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
			Vector3 rightBottomPos = CornerPosition(currentPos, kRightBottom);
			rightBottomPos.y -= kGroundSearchHeight;

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(rightBottomPos);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		// 空中状態の処理
		// 着地フラグ
		if (info.isGround) {
			// 着地状態に切り替える
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}

/*-----------------------------
 ワールド座標を取得
 -------------------------------*/
KamataEngine::Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	KamataEngine::Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

/*-------------------------
AABBを取得
--------------------------*/
AABB Player::GetAABB() {
	KamataEngine::Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};

	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

/*-----------------
衝突応答
---------------------*/

void Player::OnCollision(const Enemy* enemy) {
	(void)*enemy;

	if (IsAttack() || behavior_ == Behavior::kKnockback) {
		return;
	}

	isDead_ = true;
}

/*-------------------------------------
更新
--------------------------------*/
void Player::Update() {


	ApplyGlobalVariables();

	if (isKnockbackRequest_) {
		behaviorRequest_ = Behavior::kKnockback;
		isKnockbackRequest_ = false;
	}

	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;

		// 各振る舞いごとの初期化
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;

		case Behavior::kKnockback:
			BehaviorKnockbackInitialize();
			break;
		}

		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	// 現在の状態のUpdate
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kKnockback:
		BehaviorKnockbackUpdate();
		break;

	}
}

/*----------------------
通常の行動
----------------------------*/
void Player::BehaviorRootUpdate() {

	InputMove();

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.moveAmount = velocity_;

	CheckMapCollision(collisionMapInfo);

	velocity_ = collisionMapInfo.moveAmount;

	CheckCeilingCollision(collisionMapInfo);

	SwitchOnGround(collisionMapInfo);

	// 旋回処理
	float destinationRotationYTable[] = {
	    std::numbers::pi_v<float> / 2.0f,        // 右
	    std::numbers::pi_v<float> * 3.0f / 2.0f, // 左
	};

	float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
	// 自キャラの角度を設定する
	worldTransform_.rotation_.y = destinationRotationY;

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 攻撃ビヘイビアをリクエスト
		behaviorRequest_ = Behavior::kAttack;
	}

	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// 行列の計算
	UpdateWorldTransform(worldTransform_);
}

/*-------------------------------------
攻撃行動更新
--------------------------------*/
void Player::BehaviorAttackUpdate() {
	// アニメーションの時間を定義
	const uint32_t kChargeTime = 10;
	const uint32_t kDashTime = 3;
	const uint32_t kRecoveryTime = 20;

	// 攻撃用の速度
	KamataEngine::Vector3 attackVelocity = {0.0f, 0.0f, 0.0f};
	const float kDashSpeed = 1.5f;

	// カウンターを先に進める
	attackParameter_++;

	// サブフェーズごとの処理
	switch (attackPhase_) {
	case AttackPhase::kCharge: {
		// 溜め動作のアニメーション
		float t = static_cast<float>(attackParameter_) / kChargeTime;
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);

		if (attackParameter_ >= kChargeTime) {
			attackPhase_ = AttackPhase::kDash;
			attackParameter_ = 0; // カウンターをリセット
		}
		break;
	}
	case AttackPhase::kDash: {

		float t = static_cast<float>(attackParameter_) / kDashTime;
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);

		// 移動のコントロール
		if (lrDirection_ == LRDirection::kRight) {
			attackVelocity.x = kDashSpeed;
		} else {
			attackVelocity.x = -kDashSpeed;
		}

		if (attackParameter_ >= kDashTime) {
			attackPhase_ = AttackPhase::kRecovery;
			attackParameter_ = 0;
		}
		break;
	}
	case AttackPhase::kRecovery: {
		float t = static_cast<float>(attackParameter_) / kRecoveryTime;
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);

		if (attackParameter_ >= kRecoveryTime) {
			behaviorRequest_ = Behavior::kRoot; // 通常状態へ戻るリクエスト
			// スケールを確実に戻しておく
			worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		}
		break;
	}
	}

	// 衝突判定用に情報をセット
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.moveAmount = attackVelocity;

	// 壁などのマップ当たり判定（既存の関数を利用）
	CheckMapCollision(collisionMapInfo);

	// 実際の移動を適用
	worldTransform_.translation_.x += collisionMapInfo.moveAmount.x;
	worldTransform_.translation_.y += collisionMapInfo.moveAmount.y;
	worldTransform_.translation_.z += collisionMapInfo.moveAmount.z;

	// 行列の更新
	UpdateWorldTransform(worldTransform_);

	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;

	UpdateWorldTransform(worldTransformAttack_);
}

/*-------------------------------------
ノックバック行動
--------------------------------*/
void Player::BehaviorKnockbackUpdate() {
	// フェーズごとの処理
	switch (knockbackPhase_) {
	case KnockbackPhase::kBlow:
		// 摩擦で横移動を少しずつ減速させる
		velocity_.x *= (1.0f - kAttenuation);

		
		if (!onGround_) {
			velocity_.y -= 0.05f;
		}

		if (onGround_ && std::abs(velocity_.x) < 0.1f) {
			knockbackPhase_ = KnockbackPhase::kRecover;
			knockbackTimer_ = 0;
		}
		break;

	case KnockbackPhase::kRecover:
		
		velocity_ = {0.0f, 0.0f, 0.0f};
		knockbackTimer_++;

		const uint32_t kRecoverTime = 20; // 硬直時間
		if (knockbackTimer_ >= kRecoverTime) {
			behaviorRequest_ = Behavior::kRoot; 
		}
		break;
	}

	// 当たり判定と移動処理
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.moveAmount = velocity_;

	CheckMapCollision(collisionMapInfo);
	velocity_ = collisionMapInfo.moveAmount;
	CheckCeilingCollision(collisionMapInfo);
	SwitchOnGround(collisionMapInfo);

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	UpdateWorldTransform(worldTransform_);
}

/*--------------------
描画

--------------------*/
void Player::Draw() {
	model_->Draw(worldTransform_, *camera_);

	if (behavior_ == Behavior::kAttack) {

		if (modelAttack_) {
			modelAttack_->Draw(worldTransformAttack_, *camera_);
		}
	}
}
