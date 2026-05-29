#include "GameScene.h"
#include <imgui.h>
#include <cctype>


using namespace KamataEngine;

// デストラクタで全て解放
GameScene::~GameScene() {
	delete bloom_;
	delete player_;
	// 配列の中身をすべてループで削除
	for (NeonSign* sign : neonSigns_) {
		delete sign;
	}
}

void GameScene::Initialize() {
	bloom_ = new Bloom();
	bloom_->Initialize(1280, 720);


	/*--------------------------------
	ネオンの画像
	---------------------------------------*/
	/*
	NeonImage* image1 = new NeonImage();
	image1->Initialize(L"sample2.png");

	image1->SetTransform({0.0f, 0.0f, 0.0f}, 2.5f, 0.0f);
	image1->SetLuminanceSettings(0.8f, 12.0f);
	neonImages_.push_back(image1);
	*/

	/*
	NeonImage* image2 = new NeonImage();
	image2->Initialize(L"sample3.png");

	image2->SetTransform({3.0f, 0.0f, 0.0f}, 2.0f, 0.0f);
	image2->SetLuminanceSettings(0.6f, 10.0f);
	neonImages_.push_back(image2);


	NeonImage* neonTextH = new NeonImage();
	neonTextH->Initialize(L"H.png"); // ペイントソフトで作った文字画像
	neonTextH->SetTransform({0.0f, 3.0f, 0.0f}, 1.0f, 0.0f);

	neonTextH->SetLuminanceSettings(0.8f, 10.0f);

	neonImages_.push_back(neonTextH);
	*/





	/*
	// 左の縦棒
	NeonSign* leftBar = new NeonSign();
	leftBar->Initialize();
	leftBar->SetTransform({-1.5f, 0.0f, 0.0f}, {1.0f, 2.0f, 1.0f}, 0.0f);
	neonSigns_.push_back(leftBar);

	// 右の縦棒
	NeonSign* rightBar = new NeonSign();
	rightBar->Initialize();
	rightBar->SetTransform({1.5f, 0.0f, 0.0f}, {1.0f, 2.0f, 1.0f}, 0.0f);
	neonSigns_.push_back(rightBar);

	// 真ん中の横棒
	NeonSign* middleBar = new NeonSign();
	middleBar->Initialize();
	middleBar->SetTransform({0.0f, 0.0f, 0.0f}, {1.0f, 1.6f, 1.0f}, 1.57f);
	neonSigns_.push_back(middleBar);
	*/

	/*
	PrintNeon("Thank you for", -6.5f, 2.0f,0.5f);
	PrintNeon("Listening", -4.0f, -1.5f,0.5f);
	*/
	
	PrintNeon("a", -3.0f, 0.0f, 0.7f);

	player_ = new NeonPlayer();
	player_->Initialize();
	player_->SetTransform({0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 1.0f});

}

void GameScene::Update() {

	KamataEngine::Input* input = KamataEngine::Input::GetInstance();



	// ==========================================
	// 1. スペースキーで弾を発射 ＆ シェイク開始
	// ==========================================
	if (input->TriggerKey(DIK_SPACE)) {
		NeonBullet* newBullet = new NeonBullet();
		newBullet->Initialize();

		// ✨ 弾の発射位置を「自機の現在の位置」にする！
		KamataEngine::Vector3 spawnPos = player_->GetPosition();

		// 自機に埋もれないように、弾を少し前（上）から出す
		spawnPos.y += 0.5f;

		// 弾を配置（スケールは小さくする）
		newBullet->SetTransform(spawnPos, {0.3f, 0.3f, 1.0f});

		bullets_.push_back(newBullet);

		shakeTimer_ = 5;
	}

	// ==========================================
	// 2. シェイク（画面揺れ）の計算
	// ==========================================
	float shakeX = 0.0f;
	float shakeY = 0.0f;
	if (shakeTimer_ > 0) {
		// 画面をガクガク揺らす乱数計算
		shakeX = ((rand() % 100) / 50.0f - 1.0f) * 0.1f;
		shakeY = ((rand() % 100) / 50.0f - 1.0f) * 0.1f;
		shakeTimer_--;
	}
	// ==========================================
	// 1. ImGuiの表示とパラメータの受付
	// ==========================================
#ifdef USE_IMGUI

	ImGui::Begin("Neon Settings");


	ImGui::PushID("Neon Text Settings");
	ImGui::Text("Neon");

	// ★追加：マスター変数を操作するスライダー
	ImGui::SliderFloat("Tube Length", &globalTubeLength_, 0.0f, 1.0f);
	player_->DrawImGui();
	ImGui::Separator();
	ImGui::PopID();

	ImGui::End();

#endif

	// ==========================================
	// 2. 全てのネオンオブジェクトの更新
	// ==========================================
	for (NeonSign* sign : neonSigns_) {
		// ★追加：ImGuiで決めたマスターの長さを、それぞれの棒にセットする
		// ※ NeonSignクラスに SetTubeLength() という関数を作っておく必要があります
		sign->SetTubeLength(globalTubeLength_);

		sign->Update();
	}

	player_->Update(shakeX, shakeY);

	for (NeonImage* img : neonImages_) {
		img->Update();
	}

	for (auto it = bullets_.begin(); it != bullets_.end();) {
		(*it)->Update(shakeX, shakeY); // 弾の更新（シェイク値を渡す）

		if ((*it)->IsDead()) {
			delete *it;              // メモリを解放
			it = bullets_.erase(it); // リストから抜き取る
		} else {
			++it;
		}
	}
}

void GameScene::Draw() {

	for (NeonImage* img : neonImages_) {
		img->Draw();
	}

	bloom_->PreDraw();
	player_->Draw();

	for (NeonBullet* bullet : bullets_) {
		bullet->Draw();
}


for (NeonImage* img : neonImages_) {
		img->DrawLuminance();
	}

 //HDRキャンバスに全てのネオンを描画
	for (NeonSign* sign : neonSigns_) {
	sign->Draw();
	}

	bloom_->PostDraw();

	bloom_->Execute();
	bloom_->DrawResult();
}

// ==========================================
// 指定された文字列を、自動で横に並べて配置する関数
// ==========================================
void GameScene::PrintNeon(const std::string& text, float startX, float startY, float scale) {
	float currentX = startX;

	// ★変更：文字の間隔も、指定されたサイズ（scale）に合わせて縮小・拡大する！
	float letterSpacing = 2.0f * scale;

	for (char c : text) {
		if (c == ' ') {
			currentX += letterSpacing;
			continue;
		}

		// ★変更：工場にもサイズ（scale）を伝える
		CreateLetter(c, currentX, startY, scale);

		currentX += letterSpacing;
	}
}



// ==========================================
// 1文字ごとの「棒の組み合わせ」を定義する工場
// ==========================================
void GameScene::CreateLetter(char c, float baseX, float baseY, float scale) {

	// ==========================================
	// 💡 魔法のラムダ式を改造
	// ==========================================
	auto addBar = [&](float ox, float oy, float len, float rot) {
		NeonSign* bar = new NeonSign();
		bar->Initialize();

		// ★変更：位置のズレ(ox, oy) と、棒の太さ・長さ(1.0f, len) のすべてに scale を掛ける！
		bar->SetTransform({baseX + (ox * scale), baseY + (oy * scale), 0.0f}, {1.0f * scale, len * scale, 1.0f}, rot);
		neonSigns_.push_back(bar);
	};

	// ==========================================
	// 💡 よく使う定型パーツ（デジタル時計のようなセグメント）
	// ==========================================
	auto vl = [&]() { addBar(-0.75f, 0.0f, 2.0f, 0.0f); };   // 左の縦棒（全体）
	auto vr = [&]() { addBar(0.75f, 0.0f, 2.0f, 0.0f); };    // 右の縦棒（全体）
	auto vm = [&]() { addBar(0.0f, 0.0f, 2.0f, 0.0f); };     // 中央の縦棒（全体）
	auto ht = [&]() { addBar(0.0f, 1.0f, 1.5f, 1.57f); };    // 上の横棒
	auto hm = [&]() { addBar(0.0f, 0.0f, 1.5f, 1.57f); };    // 真ん中の横棒
	auto hb = [&]() { addBar(0.0f, -1.0f, 1.5f, 1.57f); };   // 下の横棒
	auto vtl = [&]() { addBar(-0.75f, 0.5f, 1.0f, 0.0f); };  // 左上の短い縦棒
	auto vbl = [&]() { addBar(-0.75f, -0.5f, 1.0f, 0.0f); }; // 左下の短い縦棒
	auto vtr = [&]() { addBar(0.75f, 0.5f, 1.0f, 0.0f); };   // 右上の短い縦棒
	auto vbr = [&]() { addBar(0.75f, -0.5f, 1.0f, 0.0f); };  // 右下の短い縦棒

	// 小文字が入力されても、大文字として処理するように変換
	c = (char)std::toupper(c);

	// ==========================================
	// 💡 A〜Z の設計図（パーツを組み合わせるだけ！）
	// ==========================================
	switch (c) {
	case 'A':
		vl();
		vr();
		ht();
		hm();
		break;
	case 'B':
		vl();
		ht();
		hm();
		hb();
		vtr();
		vbr();
		break; // カクカクのB
	case 'C':
		vl();
		ht();
		hb();
		break;
	case 'D':
		vl();
		vr();
		ht();
		hb();
		break; // Oと同じ（ブロック体）
	case 'E':
		vl();
		ht();
		hm();
		hb();
		break;
	case 'F':
		vl();
		ht();
		hm();
		break;
	case 'G':
		vl();
		ht();
		hb();
		vbr();
		addBar(0.375f, 0.0f, 0.75f, 1.57f);
		break; // Gの右下の折り返し
	case 'H':
		vl();
		vr();
		hm();
		break;
	case 'I':
		vm();
		ht();
		hb();
		break; // 上下にヒゲがあるI
	case 'J':
		vr();
		hb();
		vbl();
		break;
	case 'K':
		vl();
		addBar(0.0f, 0.5f, 1.8f, -0.98f);
		addBar(0.0f, -0.5f, 1.8f, 0.98f);
		break; // 斜め線
	case 'L':
		vl();
		hb();
		break;
	case 'M':
		vl();
		vr();
		addBar(-0.375f, 0.5f, 1.25f, 0.64f);
		addBar(0.375f, 0.5f, 1.25f, -0.64f);
		break;
	case 'N':
		vl();
		vr();
		addBar(0.0f, 0.0f, 2.5f, 0.64f);
		break; // 斜め線(N)
	case 'O':
		vl();
		vr();
		ht();
		hb();
		break;
	case 'P':
		vl();
		vtr();
		ht();
		hm();
		break;
	case 'Q':
		vl();
		vr();
		ht();
		hb();
		addBar(0.4f, -0.6f, 1.2f, -0.78f);
		break; // Oに右下のヒゲ
	case 'R':
		vl();
		vtr();
		ht();
		hm();
		addBar(0.375f, -0.5f, 1.25f, 0.64f);
		break;
	case 'S':
		ht();
		hm();
		hb();
		vtl();
		vbr();
		break;
	case 'T':
		ht();
		vm();
		break;
	case 'U':
		vl();
		vr();
		hb();
		break;
	case 'V':
		addBar(-0.375f, 0.0f, 2.13f, 0.36f);
		addBar(0.375f, 0.0f, 2.13f, -0.36f);
		break;
	case 'W':
		vl();
		vr();
		addBar(-0.375f, -0.5f, 1.25f, -0.64f);
		addBar(0.375f, -0.5f, 1.25f, 0.64f);
		break;
	case 'X':
		addBar(0.0f, 0.0f, 2.5f, 0.64f);
		addBar(0.0f, 0.0f, 2.5f, -0.64f);
		break; // クロス
	case 'Y':
		addBar(-0.375f, 0.5f, 1.25f, 0.64f);
		addBar(0.375f, 0.5f, 1.25f, -0.64f);
		addBar(0.0f, -0.5f, 1.0f, 0.0f);
		break;
	case 'Z':
		ht();
		hb();
		addBar(0.0f, 0.0f, 2.5f, -0.64f);
		break; // 斜め線(Z)
	}
}