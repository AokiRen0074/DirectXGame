#include "GameScene.h"
#include "Skydome.h"

using namespace KamataEngine;

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
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

	delete model_;
	delete debugCamera_;
	delete modelSkydome_;

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

	skydome_ = std::make_unique<Skydome>();
	skydome_->camera_ = &camera_;
	skydome_->Initialize();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true); 


	Model* model = Model::CreateFromOBJ("cube", true);
	uint32_t textureHandle = TextureManager::Load("uvChecker.png");
	camera_.Initialize();
	camera_.translation_ = {18.0f, 0.0f, -60.0f};
	player_ = new Player();

	player_->Initialize(model, textureHandle, &camera_);

	// 5-2
	textureHandle_ = TextureManager::Load("cube./cube.jpg");
	model_ = Model::Create();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;

	// ブロック一個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if ((i + j) % 2 == 0) {
				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
				worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
			} else {
				worldTransformBlocks_[i][j] = nullptr;
			}
		}
	}



}

// 更新
void GameScene::Update() {
	camera_.UpdateMatrix();

	if (player_ != nullptr) {
		player_->Update();
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
		camera_.UpdateMatrix();
	}


	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			Matrix4x4 matScale = KamataEngine::MathUtility::MakeScaleMatrix(worldTransformBlock->scale_);
			Matrix4x4 matRotX = KamataEngine::MathUtility::MakeRotateXMatrix(worldTransformBlock->rotation_.x);
			Matrix4x4 matRotY = KamataEngine::MathUtility::MakeRotateYMatrix(worldTransformBlock->rotation_.y);
			Matrix4x4 matRotZ = KamataEngine::MathUtility::MakeRotateZMatrix(worldTransformBlock->rotation_.z);
			Matrix4x4 matTrans = KamataEngine::MathUtility::MakeTranslateMatrix(worldTransformBlock->translation_);

			Matrix4x4 matRot = Multiply(matRotZ, Multiply(matRotX, matRotY));
			worldTransformBlock->matWorld_ = Multiply(matScale, Multiply(matRot, matTrans));

			worldTransformBlock->TransferMatrix();
		}
	}

	// 5-3
	skydome_->Update();
}

// 描画
void GameScene::Draw() {
	if (skydome_) {
		skydome_->Draw();
	}



	if (player_ != nullptr) {
		player_->Draw();
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			model_->Draw(*worldTransformBlock, camera_);
		}
	}
}