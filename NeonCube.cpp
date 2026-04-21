#include "NeonCube.h"
#include <cassert>
#include <d3dcompiler.h>

using namespace KamataEngine;
using namespace DirectX;

void NeonCube::Initialize() {
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	// ==========================================
	// 1. ルートシグネチャの作成
	// ==========================================
	D3D12_ROOT_PARAMETER rootParams[2] = {};
	// b0 (Transform用)
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[0].Descriptor.ShaderRegister = 0;
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	// b1 (Color用)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[1].Descriptor.ShaderRegister = 1;
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = 2;
	rootSigDesc.pParameters = rootParams;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	Microsoft::WRL::ComPtr<ID3DBlob> sigBlob, errBlob;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &sigBlob, &errBlob);
	assert(SUCCEEDED(hr));
	hr = device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	// ==========================================
	// 2. パイプラインステートの作成
	// ==========================================
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob;
	hr = D3DReadFileToBlob(L"NeonObject.VS.cso", &vsBlob);
	assert(SUCCEEDED(hr));
	hr = D3DReadFileToBlob(L"NeonObject.PS.cso", &psBlob);
	assert(SUCCEEDED(hr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = rootSignature_.Get();
	psoDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	psoDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	psoDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	psoDesc.PS.BytecodeLength = psBlob->GetBufferSize();
	psoDesc.InputLayout.pInputElementDescs = inputElementDescs;
	psoDesc.InputLayout.NumElements = _countof(inputElementDescs);
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT; // ★重要: BloomキャンバスのHDRフォーマットに合わせる
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.SampleDesc.Count = 1;

	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));

	// ==========================================
	// バッファ作成の共通設定
	// ==========================================
	D3D12_HEAP_PROPERTIES uploadHeap{};
	uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
	D3D12_RESOURCE_DESC bufferDesc{};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ==========================================
	// 3. 頂点バッファの作成 (立方体)
	// ==========================================
	struct Vertex {
		XMFLOAT3 pos;
	};
	Vertex vertices[] = {{{-1.0f, -1.0f, -1.0f}}, {{-1.0f, 1.0f, -1.0f}}, {{1.0f, -1.0f, -1.0f}}, {{1.0f, 1.0f, -1.0f}},
	                     {{-1.0f, -1.0f, 1.0f}},  {{-1.0f, 1.0f, 1.0f}},  {{1.0f, -1.0f, 1.0f}},  {{1.0f, 1.0f, 1.0f}}};
	bufferDesc.Width = sizeof(vertices);
	hr = device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer_));
	assert(SUCCEEDED(hr));

	Vertex* vertMap = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	memcpy(vertMap, vertices, sizeof(vertices));
	vertexBuffer_->Unmap(0, nullptr);

	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(vertices);
	vbView_.StrideInBytes = sizeof(Vertex);

	// ==========================================
	// 4. インデックスバッファの作成
	// ==========================================
	uint16_t indices[] = {0, 1, 2, 2, 1, 3, 2, 3, 6, 6, 3, 7, 6, 7, 4, 4, 7, 5, 4, 5, 0, 0, 5, 1, 1, 5, 3, 3, 5, 7, 4, 0, 6, 6, 0, 2};
	bufferDesc.Width = sizeof(indices);
	hr = device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexBuffer_));
	assert(SUCCEEDED(hr));

	uint16_t* idxMap = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&idxMap));
	memcpy(idxMap, indices, sizeof(indices));
	indexBuffer_->Unmap(0, nullptr);

	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibView_.SizeInBytes = sizeof(indices);
	ibView_.Format = DXGI_FORMAT_R16_UINT;

	// ==========================================
	// 5. 定数バッファの作成
	// ==========================================
	// 行列用 (256バイトアライメント)
	bufferDesc.Width = (sizeof(TransformData) + 0xff) & ~0xff;
	hr = device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cbTransform_));
	assert(SUCCEEDED(hr));
	cbTransform_->Map(0, nullptr, reinterpret_cast<void**>(&mappedTransform_));

	// 色用
	bufferDesc.Width = (sizeof(ColorData) + 0xff) & ~0xff;
	hr = device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cbColor_));
	assert(SUCCEEDED(hr));
	cbColor_->Map(0, nullptr, reinterpret_cast<void**>(&mappedColor_));
}

void NeonCube::Update() {
	rotationY_ += 0.02f; // ゆっくり回転させる

	// カメラとプロジェクション
	XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0.0f, 2.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
	XMMATRIX world = XMMatrixRotationY(rotationY_);

	// シェーダーに送る行列の計算
	mappedTransform_->WVP = XMMatrixTranspose(world * view * proj);

	// ★ここで色と強烈な光（HDR）を設定します！
	mappedColor_->color = {1.0f, 0.0f, 1.0f, 1.0f}; // マゼンタ
	mappedColor_->intensity = 5.0f;                 // 1.0を大きく超える発光強度
}

void NeonCube::Draw() {
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();

	cmdList->SetPipelineState(pipelineState_.Get());
	cmdList->SetGraphicsRootSignature(rootSignature_.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->IASetVertexBuffers(0, 1, &vbView_);
	cmdList->IASetIndexBuffer(&ibView_);

	// 定数バッファのアドレスを直接セット
	cmdList->SetGraphicsRootConstantBufferView(0, cbTransform_->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(1, cbColor_->GetGPUVirtualAddress());

	// 描画コマンド発行 (36インデックス)
	cmdList->DrawIndexedInstanced(36, 1, 0, 0, 0);
}