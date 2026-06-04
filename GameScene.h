#pragma once
#include "Bloom.h" // ★追加
#include "KamataEngine.h"
#include "NeonCube.h"
#include "NeonSign.h"
#include <vector>
#include "NeonImage.h"
#include "NeonPlayer.h"
#include <list>
#include "NeonBullet.h"

class GameScene {
public:
	~GameScene(); // ★メモリリーク防止のためにデストラクタを追加

	void Initialize();
	void Update();
	void Draw();

void PrintNeon(const std::string& text, float startX, float startY, float scale);
	void CreateLetter(char c, float baseX, float baseY, float scale);



private:
	Bloom* bloom_ = nullptr; 
	NeonCube* neonCube_ = nullptr;
	NeonSign* neonSign_ = nullptr;
	NeonImage* neonImage_ = nullptr;
	std::vector<NeonSign*> neonSigns_;
	std::vector<NeonImage*> neonImages_;

	float globalTubeLength_ = 2.0f;

	NeonPlayer* player_ = nullptr;

	std::list<NeonBullet*> bullets_;
	int shakeTimer_ = 0;

};