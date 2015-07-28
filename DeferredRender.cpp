#include "stdafx.h"
#include "DeferredRender.h"
#include "VertexStructures.h"
void DeferredRender::Init()
{


	//Create GPU resources
	CreateCB();
	mQuadRender.Init();
	//Create Resource views	
	CreateViews();

	//Create binding data for PSO
	CreateRootSignature();

	//Creatre pipeline state object
	CreatePso();
	CreateLightPassPSO();

	InitWindowSizeDependentResources();
}

void DeferredRender::InitWindowSizeDependentResources()
{
	CreateRTV();
	CreateDSV();

}

void DeferredRender::CreatePso()
{

	D3D12_GRAPHICS_PIPELINE_STATE_DESC descPipelineState;
	ZeroMemory(&descPipelineState, sizeof(descPipelineState));
	ShaderObject* vs = g_ShaderManager.getShaderObj("BasicVS");
	ShaderObject* ps = g_ShaderManager.getShaderObj("BasicPS");
	descPipelineState.VS = { vs->binaryPtr,vs->size };
	descPipelineState.PS = { ps->binaryPtr,ps->size };
	descPipelineState.InputLayout.pInputElementDescs = desNormalVertex;
	descPipelineState.InputLayout.NumElements = _countof(desNormalVertex);
	descPipelineState.pRootSignature = m_rootSignature.Get();
	descPipelineState.DepthStencilState=CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	descPipelineState.SampleMask = UINT_MAX;
	descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	descPipelineState.NumRenderTargets = numRTV;
	descPipelineState.RTVFormats[0] = mRtvFormat[0];
	descPipelineState.RTVFormats[1] = mRtvFormat[1];
	descPipelineState.RTVFormats[2] = mRtvFormat[2];
	descPipelineState.DSVFormat = mDsvFormat;
	descPipelineState.SampleDesc.Count = 1;

	ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pipelineState)));
	
}

void DeferredRender::CreateCB()
{
	CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = sizeof(CameraData);
	resourceDesc.Height = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mViewCB.GetAddressOf())));
	
	resourceDesc.Width = sizeof(LightData);
	ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mLightCB.GetAddressOf())));

}

void DeferredRender::CreateViews()
{
	
	m_cbvsrvHeap.Create(g_d3dObjects->GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10, true);
	//Camera CBV
	D3D12_CONSTANT_BUFFER_VIEW_DESC	descBuffer;
	descBuffer.BufferLocation = mViewCB->GetGPUVirtualAddress();
	//Constant buffer must be larger than 256 bytes
	descBuffer.SizeInBytes = (sizeof(CameraData) + 255) & ~255;
	g_d3dObjects->GetD3DDevice()->CreateConstantBufferView(&descBuffer, m_cbvsrvHeap.hCPU(0));
	//Light CBV
	descBuffer.BufferLocation = mLightCB->GetGPUVirtualAddress();
	descBuffer.SizeInBytes = (sizeof(LightData) + 255) & ~255;
	g_d3dObjects->GetD3DDevice()->CreateConstantBufferView(&descBuffer, m_cbvsrvHeap.hCPU(1));
}

void DeferredRender::CreateDSV()
{
	//Create DSV 
	m_dsvHeap.Create(g_d3dObjects->GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV,1);

	CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = mDsvFormat;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = (UINT)g_d3dObjects->GetScreenViewport().Width;
	resourceDesc.Height = (UINT)g_d3dObjects->GetScreenViewport().Height;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearVal;
	clearVal = { mDsvFormat , mClearDepth };

	ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE , &clearVal, IID_PPV_ARGS(mDsTexture.GetAddressOf())));
	D3D12_DEPTH_STENCIL_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Texture2D.MipSlice = 0;
	desc.Format = resourceDesc.Format;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_DSV_FLAG_NONE;

	g_d3dObjects->GetD3DDevice()->CreateDepthStencilView(mDsTexture.Get(),&desc,m_dsvHeap.hCPU(0));
	
	D3D12_SHADER_RESOURCE_VIEW_DESC descSRV;

	ZeroMemory(&descSRV, sizeof(descSRV));
	descSRV.Texture2D.MipLevels = resourceDesc.MipLevels;
	descSRV.Texture2D.MostDetailedMip = 0;
	descSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	descSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	descSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


	g_d3dObjects->GetD3DDevice()->CreateShaderResourceView(mDsTexture.Get(), &descSRV, m_cbvsrvHeap.hCPU(5));


}

void DeferredRender::CreateRTV()
{
	//Create deferred buffers
	//1.Albedo
	//2.Normal
	//3.Specular + Gloss
	m_rtvHeap.Create(g_d3dObjects->GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3);
	CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.MipLevels = 1;
	
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = (UINT)g_d3dObjects->GetScreenViewport().Width;
	resourceDesc.Height = (UINT)g_d3dObjects->GetScreenViewport().Height;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clearVal;
	clearVal.Color[0] = mClearColor[0];
	clearVal.Color[1] = mClearColor[1];
	clearVal.Color[2] = mClearColor[2];
	clearVal.Color[3] = mClearColor[3];

	
	

	for (int i = 0; i < numRTV; i++) {
		resourceDesc.Format = mRtvFormat[i];
		clearVal.Format = mRtvFormat[i];
		ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearVal, IID_PPV_ARGS(mRtvTexture[i].GetAddressOf())));
	}

	D3D12_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Texture2D.MipSlice = 0;
	desc.Texture2D.PlaneSlice = 0;

	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	
	for (int i = 0; i < numRTV; i++) {
		desc.Format = mRtvFormat[i];
		g_d3dObjects->GetD3DDevice()->CreateRenderTargetView(mRtvTexture[i].Get(), &desc, m_rtvHeap.hCPU(i));
	}


	//Create SRV for RTs

	D3D12_SHADER_RESOURCE_VIEW_DESC descSRV;

	ZeroMemory(&descSRV, sizeof(descSRV));
	descSRV.Texture2D.MipLevels = resourceDesc.MipLevels;
	descSRV.Texture2D.MostDetailedMip = 0;
	descSRV.Format = resourceDesc.Format;
	descSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	descSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	m_srvHeap.Create(g_d3dObjects->GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3,true);

	for (int i = 0; i < numRTV; i++) {
		descSRV.Format = mRtvFormat[i];
		g_d3dObjects->GetD3DDevice()->CreateShaderResourceView(mRtvTexture[i].Get(), &descSRV, m_cbvsrvHeap.hCPU(i + 2));
	}


}



void DeferredRender::ApplyGBufferPSO(ID3D12GraphicsCommandList * command,bool bSetPSO)
{
	ID3D12DescriptorHeap* ppHeaps[1] = { m_cbvsrvHeap.pDH.Get() };
	if (bSetPSO)
	{
		command->SetPipelineState(m_pipelineState.Get());
	}


	for (int i = 0; i < numRTV; i++)
	command->ClearRenderTargetView(m_rtvHeap.hCPU(i), mClearColor, 0, nullptr);

	command->ClearDepthStencilView(m_dsvHeap.hCPUHeapStart, D3D12_CLEAR_FLAG_DEPTH, mClearDepth, 0xff, 0, nullptr);

	command->OMSetRenderTargets(numRTV, &m_rtvHeap.hCPUHeapStart,true, &m_dsvHeap.hCPUHeapStart);
	command->SetDescriptorHeaps(1, ppHeaps);
	command->SetGraphicsRootSignature(m_rootSignature.Get());
	command->SetGraphicsRootDescriptorTable(0, m_cbvsrvHeap.hGPU(0));
	command->SetGraphicsRootDescriptorTable(1, m_cbvsrvHeap.hGPU(1));
	command->SetGraphicsRootDescriptorTable(2, m_cbvsrvHeap.hGPU(2));

}

void DeferredRender::ApplyLightingPSO(ID3D12GraphicsCommandList * command, bool bSetPSO)
{

	for (int i = 0; i < numRTV; i++)
		AddResourceBarrier(command, mRtvTexture[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);

	AddResourceBarrier(command, mDsTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_GENERIC_READ);

	command->SetPipelineState(mLightPso.Get());
	
	mQuadRender.Draw(command);

	
}

void DeferredRender::UpdateConstantBuffer(CameraData& camData, LightData& ligData)
{
	void* mapped = nullptr;
	mViewCB->Map(0, nullptr, &mapped);
	memcpy(mapped, &camData, sizeof(CameraData));
	mViewCB->Unmap(0, nullptr);

	mLightCB->Map(0, nullptr, &mapped);
	memcpy(mapped, &ligData, sizeof(LightData));
	mLightCB->Unmap(0, nullptr);	
}

void DeferredRender::CreateLightPassPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC descPipelineState;
	ZeroMemory(&descPipelineState, sizeof(descPipelineState));
	ShaderObject* vs = g_ShaderManager.getShaderObj("ScreenQuadVS");
	ShaderObject* ps = g_ShaderManager.getShaderObj("LightPassPS");
	descPipelineState.VS = { vs->binaryPtr,vs->size };
	descPipelineState.PS = { ps->binaryPtr,ps->size };
	descPipelineState.InputLayout.pInputElementDescs = desScreenQuadVertex;
	descPipelineState.InputLayout.NumElements = _countof(desScreenQuadVertex);
	descPipelineState.pRootSignature = m_rootSignature.Get();
	descPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	descPipelineState.DepthStencilState.DepthEnable = false;
	descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	descPipelineState.RasterizerState.DepthClipEnable = false;
	descPipelineState.SampleMask = UINT_MAX;
	descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	descPipelineState.NumRenderTargets = 1;
	//descPipelineState.RTVFormats[0] = mRtvFormat[0];
	descPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	descPipelineState.SampleDesc.Count = 1;
	


	ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&mLightPso)));
}

void DeferredRender::CreateRootSignature()
{
	//Init descriptor tables
	CD3DX12_DESCRIPTOR_RANGE range[3];
	//view dependent CBV
	range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	//light dependent CBV
	range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	//G-Buffer inputs
	range[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0);

	CD3DX12_ROOT_PARAMETER rootParameters[3];
	rootParameters[0].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1].InitAsDescriptorTable(1, &range[1], D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[2].InitAsDescriptorTable(1, &range[2], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.Init(3, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	CD3DX12_STATIC_SAMPLER_DESC StaticSamplers[1];
	StaticSamplers[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	descRootSignature.NumStaticSamplers = 1;
	descRootSignature.pStaticSamplers = StaticSamplers;


	ComPtr<ID3DBlob> rootSigBlob, errorBlob;

	ThrowIfFailed(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, rootSigBlob.GetAddressOf(), errorBlob.GetAddressOf()));

	ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf())));
}
