#pragma once
#include "AABB.h"

// 前方宣言
class Player;

/// 敵の基底クラス
class BaseEnemy {
public:
	// 仮想デストラクタ
	virtual ~BaseEnemy() = default;

	//派生クラスで必ず実装させる関数たち
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void OnCollision(const Player* player) = 0;
	virtual bool IsDead() const = 0;
	virtual bool IsCollisionDisabled() const = 0;
	virtual AABB GetAABB() = 0;
};