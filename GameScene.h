#pragma once
#include "CameraControll.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include <list>
#include <memory>
#include <vector>

#include "BaseEnemy.h"
#include "BaseEffect.h"

class Enemy;
class DeathParticles;
class HitEffect;
class ShieldEnemy;
class GuardEffect;
class StageManager;

// ゲームシーン
class GameScene {
public:
	;

	enum class Phase {
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};

	// エフェクトを生成する関数
	void CreateHitEffect(const KamataEngine::Vector3& position);

	Player* player_ = nullptr;
	std::list<BaseEnemy*> enemies_;
	KamataEngine::Camera camera_;
	MapChipField* mapChipField_;
	CameraController* cameraController_ = nullptr;
	// デスパーティクル
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;

	GameScene() = default;
	~GameScene();

	Phase phase_ = Phase::kFadeIn;

	// 終了フラグのゲッター
	bool IsFinished() const { return finished_; }

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3dモデル
	KamataEngine::Model* model_ = nullptr;

	// 天球3Dモデル
	KamataEngine::Model* modelSkydome_ = nullptr;

	// ヒットエフェクトモデル
	KamataEngine::Model* modelHitEffect_ = nullptr;
	KamataEngine::Model* modelGuardEffect_ = nullptr;

	std::list<BaseEffect*> effects_;

	// 普通の敵
	KamataEngine::Model* modelEnemy_ = nullptr;

	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelPlayerAttack_ = nullptr;

	// シールドのモデル
	KamataEngine::Model* modelShieldEnemy_ = nullptr;
	std::list<ShieldEnemy*> shieldEnemies_;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	Fade* fade_ = nullptr;

	// ステージマネージャー
	StageManager* stageManager_ = nullptr;

	std::unique_ptr<Skydome> skydome_;

	// 終了フラグ
	bool finished_ = false;

	// ほっとリロード要求
	bool reloadRequested_ = false;

public:
	// 初期化
	void Initialize(StageManager* stageDataManager);
	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateFieldObjects();

	// 全ての当たり判定を行う
	void CheckAllCollisions();

	// 各フェーズの更新処理をまとめる関数
	void UpdateGamePlayPhase();
	void UpdateDeathPhase();

	// フェーズ切り替えよう
	void ChangePhase();

	void CreateGuardEffect(const KamataEngine::Vector3& position);

	bool IsReloadRequested() const { return reloadRequested_; }
};