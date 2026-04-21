#include "NeonSign.h"
#include <cassert>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

void NeonSign::Initialize() {
	ID3D12Device* device = KamataEngine::DirectXCommon::GetInstance()->GetDevice();
	HRESULT hr;

	// --- 1. 頂点・インデックスバッファ作成 ---
	Vertex vertices[] = {
	    {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
	    {{-1.0f, 1.0f, 0.0f},  {0.0f, 0.0f}},
	    {{1.0f, -1.0f, 0.0f},  {1.0f, 1.0f}},
	    {{1.0f, 1.0f, 0.0f},   {1.0f, 0.0f}},
	};
	uint16_t indices[] = {0, 1, 2, 1, 3, 2};

	D3D12_HEAP_PROPERTIES uploadHeap = {D3D12_HEAP_TYPE_UPLOAD};
	D3D12_RESOURCE_DESC bufDesc = {};
	bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufDesc.Width = sizeof(vertices);
	bufDesc.Height = 1;
	bufDesc.DepthOrArraySize = 1;
	bufDesc.MipLevels = 1;
	bufDesc.SampleDesc.Count = 1;
	bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer_));
	assert(SUCCEEDED(hr));
	void* pV;
	vertexBuffer_->Map(0, nullptr, &pV);
	memcpy(pV, vertices, sizeof(vertices));
	vertexBuffer_->Unmap(0, nullptr);
	vbView_ = {vertexBuffer_->GetGPUVirtualAddress(), (UINT)sizeof(vertices), (UINT)sizeof(Vertex)};

	bufDesc.Width = sizeof(indices);
	hr = device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexBuffer_));
	assert(SUCCEEDED(hr));
	void* pI;
	indexBuffer_->Map(0, nullptr, &pI);
	memcpy(pI, indices, sizeof(indices));
	indexBuffer_->Unmap(0, nullptr);
	ibView_ = {indexBuffer_->GetGPUVirtualAddress(), (UINT)sizeof(indices), DXGI_FORMAT_R16_UINT};

	// --- 2. 定数バッファ作成 ---
	bufDesc.Width = (sizeof(TransformData) + 255) & ~255;
	device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cbTransform_));
	cbTransform_->Map(0, nullptr, (void**)&mappedTransform_);

	bufDesc.Width = (sizeof(ColorData) + 255) & ~255;
	device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cbColor_));
	cbColor_->Map(0, nullptr, (void**)&mappedColor_);

	// --- 3. ルートシグネチャ作成 ---
	D3D12_ROOT_PARAMETER rp[2] = {};
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rp[0].Descriptor.ShaderRegister = 0; // b0
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rp[1].Descriptor.ShaderRegister = 1; // b1
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rsD = {2, rp, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT};
	ComPtr<ID3DBlob> rsBlob, errBlob;
	D3D12SerializeRootSignature(&rsD, D3D_ROOT_SIGNATURE_VERSION_1, &rsBlob, &errBlob);
	device->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));

	// --- 4. シェーダー読み込み ---
	ComPtr<ID3DBlob> vsBlob, psBlob;
	if (FAILED(D3DReadFileToBlob(L"NeonObject.VS.cso", &vsBlob)) || FAILED(D3DReadFileToBlob(L"NeonObject.PS.cso", &psBlob))) {
		MessageBox(nullptr, L"Shader CSO files not found!", L"Error", MB_OK);
		return;
	}

	// --- 5. パイプラインステート(PSO)作成 ---
	D3D12_INPUT_ELEMENT_DESC layout[] = {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature_.Get();
	psoDesc.VS = {vsBlob->GetBufferPointer(), vsBlob->GetBufferSize()};
	psoDesc.PS = {psBlob->GetBufferPointer(), psBlob->GetBufferSize()};
	psoDesc.InputLayout = {layout, _countof(layout)};

	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // ネオンなので両面
	psoDesc.RasterizerState.DepthClipEnable = TRUE;

	// ブレンド設定 (加算合成)
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE; // 加算
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	// ★重要：深度バッファの設定
	// Bloom::PreDraw で nullptr を渡していても、エンジン側で何かが設定されている場合、
	// ここで正しい形式（D24_S8）を指定しないとエラーになることがあります。
	psoDesc.DepthStencilState.DepthEnable = FALSE;     // 計算はしないが、
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // 形式はエンジンに合わせる

	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;

	// ★Bloomのテクスチャ形式。R16G16B16A16 で作っていればこれでOKです。
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;

	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.SampleDesc.Count = 1;

	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));
	if (FAILED(hr)) {
		// ここで止まったら、出力ウィンドウの「D3D12 ERROR」の文字を探して教えてください！
		assert(false);
	}
}


void NeonSign::SetTransform(XMFLOAT3 pos, XMFLOAT3 scale, float rotZ) {
	position_ = pos;
	scale_ = scale;
	rotationZ_ = rotZ; // ★Z軸回転を保存
}

void NeonSign::Update() {
	// ★ 回転を XMMatrixRotationZ に変更！
	XMMATRIX world = XMMatrixScaling(scale_.x, scale_.y, scale_.z) * XMMatrixRotationZ(rotationZ_) * XMMatrixTranslation(position_.x, position_.y, position_.z);

	XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0, 0, -10, 1), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0));
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	mappedTransform_->WVP = XMMatrixTranspose(world * view * proj);

	// ネオンの質感設定
	mappedColor_->color = {0.0f, 0.8f, 1.0f, 1.0f};
	mappedColor_->intensity = 8.0f; // ★白飛びしすぎないように少し下げる
	mappedColor_->radius = 0.03f;   // ★管をさらに細くする
	mappedColor_->softness = 15.0f;
	mappedColor_->length = scale_.y;
}

void NeonSign::Draw() {
	ID3D12GraphicsCommandList* cmdList = KamataEngine::DirectXCommon::GetInstance()->GetCommandList();
	cmdList->SetPipelineState(pipelineState_.Get());
	cmdList->SetGraphicsRootSignature(rootSignature_.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView_);
	cmdList->IASetIndexBuffer(&ibView_);
	cmdList->SetGraphicsRootConstantBufferView(0, cbTransform_->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(1, cbColor_->GetGPUVirtualAddress());
	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}