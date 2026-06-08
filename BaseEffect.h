#pragma once


class BaseEffect {
public:
	// 仮想デストラクタ
	virtual ~BaseEffect() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual bool IsDead() const = 0;
};