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

	#ifdef USE_IMGUI
	KamataEngine::ImGuiManager::GetInstance()->Initialize();
#endif

	while (true) {




		if (KamataEngine::Update()) {
			break;
		}

		#ifdef USE_IMGUI
		KamataEngine::ImGuiManager::GetInstance()->Begin();
#endif

		gameScene->Update();

				#ifdef USE_IMGUI
		KamataEngine::ImGuiManager::GetInstance()->End();
#endif

		// 描画処理
		dxCommon->PreDraw();



		gameScene->Draw();

		#ifdef USE_IMGUI
		KamataEngine::ImGuiManager::GetInstance()->Draw();
#endif

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
