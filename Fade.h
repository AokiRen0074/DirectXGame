#pragma once
#include "KamataEngine.h"

class Fade {
public:

	enum class Status {
		None,    
		FadeIn,  
		FadeOut, 
	};

	Fade() = default;
	~Fade();

	void Initialize();
	void Update();
	void Draw();
	void Start(Status status, float duration);
	// フェード停止
	void Stop();
	// フェード終了判定
	bool IsFinished() const;

private:
	// フェード用黒スプライト
	KamataEngine::Sprite* sprite_ = nullptr;

	// フェードの状態
	Status status_ = Status::None;
	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;
};