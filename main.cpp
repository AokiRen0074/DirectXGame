#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;
DirectXCommon* dxCommon = DirectXCommon::GetInstance();




// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {


	KamataEngine::Initialize(L"LC1B_01_アオキ_レン_AL2");

	GameScene* gameScene = new GameScene();
	gameScene->Initialize();


	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		gameScene->Update();

		// 描画処理
		dxCommon->PreDraw();

		gameScene->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
