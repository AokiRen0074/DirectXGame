#include "CameraControll.h"
#include "Player.h"
#include <algorithm>
using namespace KamataEngine;

static KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, float t) {
	KamataEngine::Vector3 result;
	result.x = start.x + (end.x - start.x) * t;
	result.y = start.y + (end.y - start.y) * t;
	result.z = start.z + (end.z - start.z) * t;
	return result;
}

void CameraController::Initialize() { camera_.Initialize(); }

void CameraController::Update() {


	if (target_->isDead_) {
	
	camera_.UpdateMatrix();
		return;
	}


	// モードごとの移動処理
	if (mode_ == Mode::kFollow) {

		const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
		const Vector3& targetVelocity = target_->GetVelocity();

		targetPos_.x = targetWorldTransform.translation_.x + targetOffset_.x + (targetVelocity.x * kVelocityBias);
		targetPos_.y = targetWorldTransform.translation_.y + targetOffset_.y + (targetVelocity.y * kVelocityBias);
		targetPos_.z = targetWorldTransform.translation_.z + targetOffset_.z + (targetVelocity.z * kVelocityBias);

		camera_.translation_ = Lerp(camera_.translation_, targetPos_, kInterpolationRate);

		// マージン制限
		camera_.translation_.x = std::clamp(camera_.translation_.x, targetWorldTransform.translation_.x + margin_.left, targetWorldTransform.translation_.x + margin_.right);
		camera_.translation_.y = std::clamp(camera_.translation_.y, targetWorldTransform.translation_.y + margin_.bottom, targetWorldTransform.translation_.y + margin_.top);

	} else if (mode_ == Mode::kForcedScroll) {

		camera_.translation_.x += 0.05f; 


		const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
		camera_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	}

	camera_.translation_.x = std::clamp(camera_.translation_.x, movableArea_.left, movableArea_.right);
	camera_.translation_.y = std::clamp(camera_.translation_.y, movableArea_.bottom, movableArea_.top);

	// 行列を更新する
	camera_.UpdateMatrix();




}

void CameraController::Reset() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	camera_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}
