#include "Skydome.h"

using namespace KamataEngine;

void Skydome::Initialize() {

	model_ = Model::CreateFromOBJ("skydome", true);

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
}

void Skydome::Update() {}

void Skydome::Draw() {
	if (!camera_)
		return;
	if (camera_) {

		model_->Draw(worldTransform_, *camera_);
	}
}
