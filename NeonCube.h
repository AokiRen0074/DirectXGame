#pragma once
#include "KamataEngine.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

class NeonCube {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	// 行列と色を送るための定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> cbTransform_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cbColor_;

	struct TransformData {
		DirectX::XMMATRIX WVP;
	};

	struct ColorData {
		DirectX::XMFLOAT4 color;
		float intensity;
	};

	TransformData* mappedTransform_ = nullptr;
	ColorData* mappedColor_ = nullptr;

	float rotationY_ = 0.0f; // アニメーション用
};