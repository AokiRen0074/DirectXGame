#pragma once
#include "Bloom.h" // ★追加
#include "KamataEngine.h"
#include "NeonCube.h"
#include "NeonSign.h"
#include <vector>

class GameScene {
public:
	~GameScene(); // ★メモリリーク防止のためにデストラクタを追加

	void Initialize();
	void Update();
	void Draw();

private:
	Bloom* bloom_ = nullptr; // ★追加
	NeonCube* neonCube_ = nullptr;
	NeonSign* neonSign_ = nullptr;
	std::vector<NeonSign*> neonSigns_;
};