#pragma once
#include "DirectxHelper.h"

class ScreenQuadRenderer
{
public:
	void Init();
	void Draw(ComPtr<ID3D12GraphicsCommandList> commandList);
	
private:
	ComPtr<ID3D12Resource> mVB;

	D3D12_VERTEX_BUFFER_VIEW mVbView;

};