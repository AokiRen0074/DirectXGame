#include "GameScene.h"
#include "KamataEngine.h"
#include "Player.h"
#include "TitleScene.h"
#include <Windows.h>
#include "StageManager.h"
#include <fstream> 
#include <sstream>
#include "GlobalVariables.h"


using namespace KamataEngine;
DirectXCommon* dxCommon = DirectXCommon::GetInstance();

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

ImGuiManager* imguiManager = ImGuiManager::GetInstance();



// 現在シーン
Scene scene = Scene::kUnknown;

// 各シーンのポインタ
GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

StageManager* stageManager = nullptr;

void LoadDebugSettings() {
	// 起動設定ファイルを開く
	std::ifstream file("DebugSettings.ini");
	if (!file.is_open()) {
		// ファイルが無ければデフォルトのまま何もしない
		return;
	}

	std::string line;
	// 1行ずつ読み込む
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string key;
		std::string value;

		// 空白区切りで「キー」と「バリュー」を取得
		ss >> key >> value;

		// ステージ設定
		if (key == "InitialStage") {
			// ステージ名から番号を設定する
			stageManager->SetCurrentStageIndexByName(value);
		}
	}
	file.close();
}

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
			gameScene->Initialize(stageManager);
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
		} else if (gameScene->IsReloadRequested()) {
			// シーンリロード
			delete gameScene;
			gameScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize(stageManager);
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

	// グローバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();

	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();
	imguiManager->Initialize();



	// インスタンス
	stageManager = new StageManager;
	// ステージデータファイルを読み込む
	stageManager->LoadStageDatas();

	#ifdef _DEBUG
	// デバッグ設定ファイル読み込み
	LoadDebugSettings();

	// デバッグ時はタイトルを飛ばしてゲームシーンから開始
	scene = Scene::kGame;
	gameScene = new GameScene;
	gameScene->Initialize(stageManager);
#else
	// リリースビルド時は通常通りタイトルから
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();
#endif

	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		imguiManager->Begin();

		ChangeScene();
		UpdateScene();

		GlobalVariables::GetInstance()->Update();

		// 描画処理
		dxCommon->PreDraw();

		KamataEngine::Model::PreDraw();
		DrawScene();

		KamataEngine::Model::PostDraw();

		imguiManager->End();
		imguiManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}



	// ゲームシーンの解放
	delete titleScene;
	delete gameScene;
	titleScene = nullptr;
	delete stageManager;

	KamataEngine::Finalize();

	return 0;
}
