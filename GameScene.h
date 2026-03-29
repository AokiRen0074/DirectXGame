#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>
#include "Skydome.h"
#include <memory>
#include "MapChipField.h"
#include "CameraControll.h"
#include <list>
#include "Fade.h"


class Enemy;
class DeathParticles;
class HitEffect;

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
	std::list<Enemy*> enemies_;
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

	std::list<HitEffect*> hitEffects_;


	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	Fade* fade_ = nullptr;

	std::unique_ptr<Skydome> skydome_;

	// 終了フラグ
	bool finished_ = false;

	public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBlocks();

	// 全ての当たり判定を行う
	void CheckAllCollisions();

	// 各フェーズの更新処理をまとめる関数
	void UpdateGamePlayPhase();
	void UpdateDeathPhase();

	// フェーズ切り替えよう
	void ChangePhase();
};