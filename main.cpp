#include "GameScene.h"
#include "KamataEngine.h"
#include "Player.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;
DirectXCommon* dxCommon = DirectXCommon::GetInstance();

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

// 現在シーン
Scene scene = Scene::kUnknown;

// 各シーンのポインタ
GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;


void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"LC1B_01_アオキ_レン_AL2");

	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();





	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		ChangeScene();
		UpdateScene();

		// 描画処理
		dxCommon->PreDraw();

		KamataEngine::Model::PreDraw();
		DrawScene();

		KamataEngine::Model::PostDraw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete titleScene;
	delete gameScene;
	titleScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
