#include "GameScene.h"

using namespace KamataEngine;

// 初期化
void GameScene::Initialize() {

	Audio::GetInstance()->LoadWave("mokugyo.wav");
	Audio::GetInstance()->PlayWave(soundDataHandle_,true);
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_,true);
}


// 更新
void GameScene::Update() {

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		Audio::GetInstance()->StopWave(voiceHandle_);
	}

}

// 描画
void GameScene::Draw() {}