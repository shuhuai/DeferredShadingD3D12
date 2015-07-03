#include "stdafx.h"
#include "ScreenQuadRenderer.h"
#include  "VertexStructures.h"
#include "d3dx12.h"
void ScreenQuadRenderer::Init()
{
	ScreenQuadVertex QuadVerts[] =
	{
		{ { -1.0f,1.0f, 0.0f,1.0f },{ 0.0f,0.0f } },
		{ { 1.0f, 1.0f, 0.0f,1.0f }, {1.0f,0.0f } },
		{ { -1.0f, -1.0f, 0.0f,1.0f },{ 0.0f,1.0f } },
	{ { 1.0f, -1.0f, 0.0f,1.0f },{ 1.0f,1.0f } }
	};

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
	resourceDesc.Width = sizeof(QuadVerts);
	resourceDesc.Height = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mVB.GetAddressOf())));
	
	UINT8* dataBegin;
	ThrowIfFailed(mVB->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin)));
	memcpy(dataBegin, QuadVerts, sizeof(QuadVerts));
	mVB->Unmap(0, nullptr);

	mVbView.BufferLocation = mVB->GetGPUVirtualAddress();
	mVbView.StrideInBytes = sizeof(ScreenQuadVertex);
	mVbView.SizeInBytes = sizeof(QuadVerts);
}

void ScreenQuadRenderer::Draw(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->IASetVertexBuffers(0, 1, &mVbView);
	commandList->DrawInstanced(4, 1, 0, 0);
}
