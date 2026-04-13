#pragma once
#include "KamataEngine.h"
#include "AABB.h"
#include "BaseEnemy.h"

class Player;
class GameScene;

class Enemy final : public BaseEnemy {
public:

	enum class Behavior {
		kUnknown,
		kRoot, 
		kDeath, 
	};

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update() override;
	void Draw() override;
	void OnCollision(const Player* player) override;
	bool IsDead() const override { return isDead_; }
	bool IsCollisionDisabled() const override { return isCollisionDisabled_; }
	AABB GetAABB() override;
	
	// セッター
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();


private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	
	bool isCollisionDisabled_ = false;

	// キャラクターのサイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kWalkSpeed = 0.03f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	GameScene* gameScene_ = nullptr;

	/*-----------------
	ビヘイビア
	-----------------------*/
	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	uint32_t deathTimer_ = 0;

	void BehaviorRootInitialize();
	void BehaviorDeathInitialize();
	void BehaviorRootUpdate();
	void BehaviorDeathUpdate();

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

	bool isDead_ = false;
};