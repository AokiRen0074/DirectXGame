#include "GameScene.h"
#include "Skydome.h"
#include "WorldTransform.h"
#include "Enemy.h"
#include "AABB.h"
#include "DeathParticles.h"


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
void GameScene::Initialize() {

	/*---------------------------
	マップチップ
	------------------------------*/
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/Stage.csv");

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
	Model* playerModel = Model::CreateFromOBJ("player", true);
	camera_.Initialize();
	camera_.farZ = 2000.0f;
	camera_.translation_ = {0.0f, 0.0f, -50.0f};
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(0, 0);
	player_->Initialize(playerModel, &camera_,playerPosition);
	player_->worldTransform_.translation_ = {5.0f, 3.0f, 0.0f};
	//player_->worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	player_->SetMapChipField(mapChipField_);

	/*-----------------------
	エネミー
	-----------------------------*/
	Model* enemyModel = Model::CreateFromOBJ("enemy", true); 
	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		
		Vector3 enemyPosition = {15.0f + (i * 5.0f), 3.0f, 0.0f};
		newEnemy->Initialize(enemyModel, &camera_, enemyPosition);

		// リストに追加！
		enemies_.push_back(newEnemy);
	}

	/*------------------------
	デスパーティクル
	-------------------------*/
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle", true);

	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(modelDeathParticle_, &camera_, player_->worldTransform_.translation_);

	/*--------------------
	追従カメラ
	-------------------------*/
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);

	CameraController::Rect cameraArea = {10.2f, 100.0f, 6.0f, 100.0f}; 
	cameraController_->SetMovableArea(cameraArea);

	cameraController_->Reset();
	



	// 5-0

	model_ = Model::CreateFromOBJ("block", true);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	debugCamera_->SetFarZ(2000.0f);


	GenerateBlocks();

}


void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 配列の大きさを設定する
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
		
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();

				//worldTransform->scale_ = {2.0f, 2.0f, 2.0f};


				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j] = worldTransform;
			} else {
				worldTransformBlocks_[i][j] = nullptr;
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
			// 敵の座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2)) {
				
				player_->OnCollision(enemy);
				enemy->OnCollision(player_);
			}
		}
	}
#pragma endregion

#pragma region 自キャラとアイテムの当たり判定

#pragma endregion

#pragma region 自弾と敵キャラの当たり判定

#pragma endregion
}



// 更新
void GameScene::Update() {
	camera_.UpdateMatrix();

	if (player_ != nullptr) {
		player_->Update();
	}

	if (cameraController_ != nullptr) {
		cameraController_->Update();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	if (deathParticles_) {
		deathParticles_->Update();
	}

	// 5-2

	debugCamera_->Update();

	#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_P)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

	#endif

	// デバッグカメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();

		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;

		// 行列を転送して反映させる
		camera_.TransferMatrix();
	} else {
		camera_.matView = cameraController_->GetCamera().matView;
		camera_.matProjection = cameraController_->GetCamera().matProjection;

		// 行列を転送して反映させる
		camera_.TransferMatrix();
	}


	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			UpdateWorldTransform(*worldTransformBlock);
		}
	}

	// 5-3
	skydome_->Update();
	CheckAllCollisions();
}

// 描画
void GameScene::Draw() {
	if (skydome_) {
		skydome_->Draw();
	}



	if (player_ != nullptr) {
		player_->Draw();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			model_->Draw(*worldTransformBlock, camera_);
		}
	}
}