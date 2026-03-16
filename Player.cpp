#include "Player.h"
#include "KamataEngine.h"
#include <cassert>
#include "WorldTransform.h"

using namespace KamataEngine;

static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	camera_ = camera;

	worldTransform_.Initialize();
}

void Player::Update() {
		
	UpdateWorldTransform(worldTransform_);
	}


void Player::Draw() {
	model_->Draw(worldTransform_, *camera_);
}
