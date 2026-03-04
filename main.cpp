#include "GameScene.h"
#include "KamataEngine.h"
#include "Player.h"
#include <Windows.h>

using namespace KamataEngine;
DirectXCommon* dxCommon = DirectXCommon::GetInstance();




// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {


	KamataEngine::Initialize(L"LC1B_01_アオキ_レン_AL2");

	Model* model = Model::CreateFromOBJ("cube", true);
	uint32_t textureHandle = TextureManager::Load("uvChecker.png");

	//　カメラ
	Camera camera;
	camera.Initialize();
	camera.translation_ = {0, 0, -10.0f};

	// プレイヤーの生成
	Player* player = new Player();
	player->Initialize(model, textureHandle, &camera);

	player->worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};


	GameScene* gameScene = new GameScene();
	gameScene->Initialize();



	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		gameScene->Update();
		player->Update();

		// 描画処理
		dxCommon->PreDraw();

		gameScene->Draw();
		player->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
