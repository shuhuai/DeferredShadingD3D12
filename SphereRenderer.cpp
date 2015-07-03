#include "stdafx.h"

#include "SphereRenderer.h"
#include "VertexStructures.h"
#include "DirectXMathConverter.h"
#include <vector>
#include <math.h>

using namespace DirectX;
void SphereRenderer::Init(float radius,int slices, int segments)
{
	mRadius = radius;
	mSlices=slices;
	mSegments=segments;

	resourceSetup();
}

void SphereRenderer::resourceSetup()
{
	

	std::vector< NormalVertex > verts;
	verts.resize((mSegments+1)*mSlices+2);

	const float _pi = XM_PI;
	const float _2pi = XM_2PI;

	verts[0].position = XMFLOAT4(0, mRadius ,0,1) ;
	for (int lat = 0; lat < mSlices; lat++)
	{
		float a1 = _pi * (float)(lat + 1) / (mSlices + 1);
		float sin1 = sinf(a1);
		float cos1 = cosf(a1);

		for (int lon = 0; lon <= mSegments; lon++)
		{
			float a2 = _2pi * (float)(lon == mSegments ? 0 : lon) / mSegments;
			float sin2 = sinf(a2);
			float cos2 = cosf(a2);

			verts[lon + lat * (mSegments + 1) + 1].position = XMFLOAT4( sin1 * cos2* mRadius, cos1* mRadius, sin1 * sin2* mRadius,1) ;
		}
	}
	verts[verts.size() - 1].position = XMFLOAT4(0, -mRadius, 0, 1);
	
	for (int n = 0; n < verts.size(); n++)
		verts[n].normal = GMathVF(XMVector3Normalize( GMathFV(XMFLOAT3(verts[n].position.x, verts[n].position.y, verts[n].position.z))));

	int nbFaces = verts.size();
	int nbTriangles = nbFaces * 2;
	int nbIndexes = nbTriangles * 3;
	std::vector< int >  triangles(nbIndexes);
	//int* triangles = new int[nbIndexes];


	int i = 0;
	for (int lon = 0; lon < mSegments; lon++)
	{
		triangles[i++] = lon + 2;
		triangles[i++] = lon + 1;
		triangles[i++] = 0;
	}

	//Middle
	for (int lat = 0; lat < mSlices - 1; lat++)
	{
		for (int lon = 0; lon < mSegments; lon++)
		{
			int current = lon + lat * (mSegments + 1) + 1;
			int next = current + mSegments + 1;

			triangles[i++] = current;
			triangles[i++] = current + 1;
			triangles[i++] = next + 1;

			triangles[i++] = current;
			triangles[i++] = next + 1;
			triangles[i++] = next;
		}
	}

	//Bottom Cap
	for (int lon = 0; lon < mSegments; lon++)
	{
		triangles[i++] = verts.size() - 1;
		triangles[i++] = verts.size() - (lon + 2) - 1;
		triangles[i++] = verts.size() - (lon + 1) - 1;
	}
	mTriangleSize= verts.size();
	 mIndexSize= triangles.size();
	//Create D3D resources
	D3D12_HEAP_PROPERTIES heapProperty;
	ZeroMemory(&heapProperty, sizeof(heapProperty));
	heapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = sizeof(NormalVertex)*verts.size();
	resourceDesc.Height = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mVertexBuffer.GetAddressOf())));

	UINT8* dataBegin;
	ThrowIfFailed(mVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin)));
	memcpy(dataBegin, &verts[0], sizeof(NormalVertex)*verts.size());
	mVertexBuffer->Unmap(0, nullptr);

	mVertexView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexView.StrideInBytes = sizeof(NormalVertex);
	mVertexView.SizeInBytes = sizeof(NormalVertex)*verts.size();

	resourceDesc.Width = sizeof(int)*triangles.size();
	ThrowIfFailed(g_d3dObjects->GetD3DDevice()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mIndexBuffer.GetAddressOf())));
	ThrowIfFailed(mIndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin)));
	memcpy(dataBegin, &triangles[0], sizeof(int)*triangles.size());
	mIndexBuffer->Unmap(0, nullptr);

	
	mIndexView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexView.Format = DXGI_FORMAT_R32_UINT;
	mIndexView.SizeInBytes= sizeof(int)*triangles.size();
	return;
}

void SphereRenderer::Render(ComPtr<ID3D12GraphicsCommandList> commandList)
{


	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetIndexBuffer(&mIndexView);
	commandList->IASetVertexBuffers(0, 1, &mVertexView);
	//commandList->DrawInstanced();
	commandList->DrawIndexedInstanced(mIndexSize,1, 0, 0,0);

	return;

}
