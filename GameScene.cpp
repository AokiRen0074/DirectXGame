#include "GameScene.h"
#include "AABB.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "GlobalVariables.h"
#include "GuardEffect.h"
#include "HitEffect.h"
#include "ShieldEnemy.h"
#include "Skydome.h"
#include "StageManager.h"
#include "WorldTransform.h"
#include "Player.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif

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

// デストラクタ
GameScene::~GameScene() {
	delete player_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	if (deathParticles_) {
		delete deathParticles_;
		deathParticles_ = nullptr;
	}
	delete modelDeathParticle_;

	enemies_.clear();

	delete model_;
	delete debugCamera_;
	delete modelSkydome_;
	delete mapChipField_;
	delete cameraController_;
	delete fade_;
	delete modelHitEffect_;
	delete modelPlayer_;
	delete modelPlayerAttack_;

	for (GuardEffect* effect : guardEffects_) {
		delete effect;
	}
	guardEffects_.clear();
	delete modelGuardEffect_;

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		delete shieldEnemy;
	}
	shieldEnemies_.clear();
	delete modelShieldEnemy_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

// 初期化
void GameScene::Initialize(StageManager* stageDataManager) {
	// 引数をメンバ変数に記録する
	stageManager_ = stageDataManager;
	/*-----------------
	調整項目の登録
	--------------------------*/
	Player::RegisterGlobalVariables();
	Enemy::RegisterGlobalVariables();
	ShieldEnemy ::RegisterGlobalVariables();
	CameraController::RegisterGlobalVariables();
	DeathParticles::RegisterGlobalVariables();
	GuardEffect::RegisterGlobalVariables();
	HitEffect::RegisterGlobalVariables();

	/*---------------------------
	マップチップ
	------------------------------*/
	mapChipField_ = new MapChipField;

	// 現在のステージデータを取得する
	const StageData& stageData = stageManager_->GetCurrentStageData();
	// ステージファイルパスの生成
	std::string stageFileName = "Resources/fields/" + stageData.name + ".csv";
	// ステージファイルの読み込み
	mapChipField_->LoadMapChipCsv(stageFileName);

	/*-------------------------------
	天球
	-------------------------------*/
	skydome_ = std::make_unique<Skydome>();
	skydome_->camera_ = &camera_;
	skydome_->Initialize();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	/*--------------------
	プレイヤー
	-------------------------*/
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	modelPlayerAttack_ = Model::CreateFromOBJ("hit_effect", true);
	camera_.Initialize();
	camera_.farZ = 2000.0f;
	camera_.translation_ = {0.0f, 0.0f, -50.0f};

	/*-----------------------
	エネミー
	-----------------------------*/
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

	/*-----------------------
	シールドエネミー
	-----------------------------*/
	modelShieldEnemy_ = Model::CreateFromOBJ("shieldEnemy", true);

	/*------------------------
	デスパーティクル
	-------------------------*/
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle", true);

	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(modelDeathParticle_, &camera_, {0.0f, 0.0f, 0.0f});

	/*------------------------
	ヒットエフェクト
	-------------------------*/
	// ヒットエフェクト用モデルの読み込み
	modelHitEffect_ = Model::CreateFromOBJ("particle", true);

	HitEffect::SetModel(modelHitEffect_);
	HitEffect::SetCamera(&camera_);

	// シールドエネミーのときの
	modelGuardEffect_ = Model::CreateFromOBJ("ring", true);
	GuardEffect::SetModel(modelGuardEffect_);
	GuardEffect::SetCamera(&camera_);

	GenerateFieldObjects();

	/*--------------------
	追従カメラ
	-------------------------*/
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);

	CameraController::Rect cameraArea = {10.2f, 100.0f, 6.0f, 100.0f};
	cameraController_->SetMovableArea(cameraArea);

	cameraController_->Reset();

	/*------------------------
	ゲームフェーズ
	--------------------------------*/
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f); // ゲーム開始時に1秒でフェードイン
	phase_ = Phase::kFadeIn;

	// 5-0

	model_ = Model::CreateFromOBJ("block", true);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	debugCamera_->SetFarZ(2000.0f);
}

void GameScene::GenerateFieldObjects() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 配列の大きさを設定する
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// フィールドオブジェクトの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {

			MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(j, i);

			switch (mapChipType) {
			case MapChipType::kBlock: {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j] = worldTransform;
				break;
			}
			case MapChipType::kPlayer: {
				assert(player_ == nullptr && "自キャラを二重に配置しようとしています");
				// 自キャラの生成
				player_ = new Player();
				// 座標を指定してキャラの初期化
				KamataEngine::Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				player_->Initialize(modelPlayer_, modelPlayerAttack_, &camera_, playerPosition);
				// 自キャラにマップチップ情報をセット
				player_->SetMapChipField(mapChipField_);

				// プレイヤーが入るマスにブロックのデータは不要なので nullptr
				worldTransformBlocks_[i][j] = nullptr;
				break;
			}
			case MapChipType::kEnemy: {
				// サブIDを取得
				uint8_t subID = mapChipField_->GetMapChipSubIDByIndex(j, i);
				// 座標を取得
				KamataEngine::Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(j, i);

				if (subID == 0) {
					// 　サブIDが0普通の敵
					Enemy* newEnemy = new Enemy();
					newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
					newEnemy->SetGameScene(this);
					enemies_.push_back(newEnemy);

				} else if (subID == 1) {
					// サブIDが1ならシールドエネミー
					ShieldEnemy* newShieldEnemy = new ShieldEnemy();
					newShieldEnemy->Initialize(modelShieldEnemy_, &camera_, enemyPosition);
					newShieldEnemy->SetGameScene(this);
					shieldEnemies_.push_back(newShieldEnemy);
				}

				worldTransformBlocks_[i][j] = nullptr;
				break;
			}
			default:

				worldTransformBlocks_[i][j] = nullptr;
				break;
			}
		}
	}
}

/*-------------------
全ての当たり判定を行う
--------------------------*/
void GameScene::CheckAllCollisions() {

	// 判定対象1と2の座標
	AABB aabb1, aabb2;

#pragma region 自キャラと敵キャラの当たり判定
	if (player_) {
		// 自キャラの座標
		aabb1 = player_->GetAABB();

		// 自キャラと敵全ての当たり判定
		for (Enemy* enemy : enemies_) {

			if (enemy->IsCollisionDisabled()) {
				continue;
			}

			// 敵の座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2)) {

				player_->OnCollision(enemy);
				enemy->OnCollision(player_);
			}
		}
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {

		if (shieldEnemy->IsCollisionDisabled()) {
			continue;
		}

		aabb2 = shieldEnemy->GetAABB();

		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollision((Enemy*)shieldEnemy);
			shieldEnemy->OnCollision(player_);
		}
	}
#pragma endregion

#pragma region 自キャラとアイテムの当たり判定

#pragma endregion

#pragma region 自弾と敵キャラの当たり判定

#pragma endregion
}

/*------------------------
フェーズの切り替え処理
------------------------*/

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		// 自キャラがデス状態かチェック
		if (player_->isDead_) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			if (deathParticles_) {
				delete deathParticles_;
			}
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:

		break;
	}
}

/*--------------------------------
ヒットエフェクト生成処理
-----------------------------------*/
void GameScene::CreateHitEffect(const KamataEngine::Vector3& position) {
	HitEffect* newHitEffect = HitEffect::Create(position);
	hitEffects_.push_back(newHitEffect);
}

/*----------------------------
更新処理
----------------------------*/
void GameScene::Update() {

	Player::ApplyGlobalVariables();
	Enemy::ApplyGlobalVariables();
	ShieldEnemy::ApplyGlobalVariables();
	CameraController::ApplyGlobalVariables();
	DeathParticles::ApplyGlobalVariables();
	GuardEffect::ApplyGlobalVariables();
	HitEffect::ApplyGlobalVariables();

#ifdef _DEBUG
	    // リロードボタン
	    // ImGui::Begin("Debug");
	    if (ImGui::Button("Reload")) {
		reloadRequested_ = true;
	}
	ImGui::End();
#endif

	camera_.UpdateMatrix();
	ChangePhase();

	switch (phase_) {
	case Phase::kFadeIn:
		// フェードイン処理
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
			fade_->Stop();
		}

		UpdateGamePlayPhase();
		break;

	case Phase::kPlay:
		UpdateGamePlayPhase();
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_L)) {
			GlobalVariables::GetInstance()->SaveFile("Player");
		}
		break;

	case Phase::kDeath:
		UpdateDeathPhase();
		// フェードアウトに移行する
		if (deathParticles_ && deathParticles_->isFinished_) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case Phase::kFadeOut:
		// フェードアウト処理
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		UpdateDeathPhase();
		break;
	}
}

/*---------------------------
ゲームプレイ
------------------------------*/
void GameScene::UpdateGamePlayPhase() {
	// 自キャラの更新
	if (player_ != nullptr) {
		player_->Update();
	}

	// カメラコントローラの更新
	if (cameraController_ != nullptr) {
		cameraController_->Update();
	}

	// 敵の更新
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// ヒットエフェクトの更新
	for (HitEffect* effect : hitEffects_) {
		effect->Update();
	}

	hitEffects_.remove_if([](HitEffect* effect) {
		if (effect->IsDead()) {
			delete effect;
			return true;
		}
		return false;
	});

	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	for (GuardEffect* effect : guardEffects_) {
		effect->Update();
	}

	guardEffects_.remove_if([](GuardEffect* effect) {
		if (effect->IsDead()) {
			delete effect;
			return true;
		}
		return false;
	});

	// シールドエネミーの更新
	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		shieldEnemy->Update();
	}

	shieldEnemies_.remove_if([](ShieldEnemy* shieldEnemy) {
		if (shieldEnemy->IsDead()) {
			delete shieldEnemy;
			return true;
		}
		return false;
	});

	// デバッグカメラの処理とカメラの更新
	debugCamera_->Update();
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_P)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

#endif
	if (isDebugCameraActive_) {
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.matView = cameraController_->GetCamera().matView;
		camera_.matProjection = cameraController_->GetCamera().matProjection;
		camera_.TransferMatrix();
	}

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			UpdateWorldTransform(*worldTransformBlock);
		}
	}

	// 天球の更新
	skydome_->Update();

	// 全ての当たり判定
	CheckAllCollisions();
}

// デス演出フェーズの更新処理
void GameScene::UpdateDeathPhase() {
	// 敵の更新
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		shieldEnemy->Update();
	}

	// デスパーティクルの更新（デスフェーズのみ）
	if (deathParticles_) {
		deathParticles_->Update();
	}

	// デバッグカメラの処理とカメラの更新
	debugCamera_->Update();
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_P)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	if (isDebugCameraActive_) {
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.matView = cameraController_->GetCamera().matView;
		camera_.matProjection = cameraController_->GetCamera().matProjection;
		camera_.TransferMatrix();
	}

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			UpdateWorldTransform(*worldTransformBlock);
		}
	}

	// 天球の更新
	skydome_->Update();
}

void GameScene::CreateGuardEffect(const KamataEngine::Vector3& position) {
	GuardEffect* newEffect = GuardEffect::Create(position);
	guardEffects_.push_back(newEffect);
}

// 描画
void GameScene::Draw() {
	if (skydome_) {
		skydome_->Draw();
	}

	if (player_ != nullptr && !player_->isDead_) {
		player_->Draw();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	for (HitEffect* effect : hitEffects_) {
		effect->Draw();
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		shieldEnemy->Draw();
	}

	for (GuardEffect* effect : guardEffects_) {
		effect->Draw();
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			model_->Draw(*worldTransformBlock, camera_);
		}
	}

	if (fade_) {
		fade_->Draw();
	}
}