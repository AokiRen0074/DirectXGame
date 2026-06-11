#pragma once
#include <KamataEngine.h>

class Player;

class CameraController {
public:

		struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	Rect movableArea_ = {0, 100, 0, 100};

	void Initialize();

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	void SetMovableArea(Rect area) { movableArea_ = area; }

		// 調整項目を登録
	static void RegisterGlobalVariables();
	static void ApplyGlobalVariables();

	const KamataEngine::Camera& GetCamera() const { return camera_; }

private:
	KamataEngine::Camera camera_;

public:
	Player* target_ = nullptr;
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};
	KamataEngine::Vector3 targetPos_;

	static inline const float kInterpolationRate = 0.1f;
	static inline const float kVelocityBias = 15.0f;
	static inline const Rect margin_ = {-15.0f, 15.0f, -10.0f, 10.0f};


};