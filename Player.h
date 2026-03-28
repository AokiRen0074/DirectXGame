#pragma once
#include "KamataEngine.h"
#include "AABB.h"

class MapChipField;
class Enemy;

class Player {
public:

	enum class LRDirection {
		kRight,
		kLeft,
	};


	struct CollisionMapInfo {
		bool isCeiling = false;        
		bool isGround = false;           
		bool isWall = false;             
		KamataEngine::Vector3 moveAmount = {};
	};

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner
	};

	enum class Behavior {
		kUnknown,
		kRoot,   
		kAttack, 
	};

	enum class AttackPhase {
		kCharge,   
		kDash,     
		kRecovery, 
	};

	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	void InputMove();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	//衝突応答
	void OnCollision(const Enemy* enemy);

	bool IsAttack() const { return behavior_ == Behavior::kAttack; }
	

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();

	AABB GetAABB();

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; };


	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	
	LRDirection lrDirection_ = LRDirection::kRight;

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.02f;
	static inline const float kAttenuation = 0.05f;
	static inline const float kLimitRunSpeed = 0.3f;

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; };

	// デスフラグ
	bool isDead_ = false;

	// 接地状態フラグ
	bool onGround_ = true;
	// 重力加速度
	static inline const float kGravityAcceleration = 0.01f;
	// 最大落下速度
	static inline const float kLimitFallSpeed = 0.5f;
	// ジャンプ初速
	static inline const float kJumpAcceleration = 0.3f;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.001f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.5f;

	// 壁接触時の速度減衰
	static inline const float kAttenuationWall = 0.5f;

	// 吸着用
	static inline const float kGroundSearchHeight = 0.06f;

	// 接地状態の切り替え処理
	void SwitchOnGround(const CollisionMapInfo& info);

	// 当たり判定
	void CheckMapCollision(CollisionMapInfo& info);
	void CheckCeilingCollision(const CollisionMapInfo& info);

	// 壁に接触している時の当たり判定
	void CheckWallCollision(const CollisionMapInfo& info);

private:
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// 現在のビヘイビア
	Behavior behavior_ = Behavior::kRoot;

	// 次の振る舞いリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	uint32_t attackParameter_ = 0;

	AttackPhase attackPhase_ = AttackPhase::kCharge;

	KamataEngine::Model* modelAttack_ = nullptr;
	KamataEngine::WorldTransform worldTransformAttack_;

	void BehaviorRootUpdate();

	void BehaviorAttackUpdate();

	// 通常行動初期化
	void BehaviorRootInitialize();
	// 攻撃行動初期化
	void BehaviorAttackInitialize();
	

	// uint32_t textureHandle_ = 0u;
};