#pragma once

#include "DirectxHelper.h"

class SphereRenderer
{
public:
	void Init(float radius,int slices,int segments);
	void resourceSetup();
	void Render(ComPtr<ID3D12GraphicsCommandList> commandList);

private:
	ComPtr<ID3D12Resource> mVertexBuffer;
	ComPtr<ID3D12Resource> mDefaultBuffer;
	ComPtr<ID3D12Resource> mIndexBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexView;
	D3D12_INDEX_BUFFER_VIEW mIndexView;
	int mSlices;
	int mSegments;
	int mTriangleSize;
	int mIndexSize;
	float mRadius;
	struct SimpleVertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
};