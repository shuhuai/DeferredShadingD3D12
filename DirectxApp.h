#include "windowsApp.h"
#include "DirectxHelper.h"
#include <stdio.h>
#include "DeferredRender.h"
#include "CameraManager.h"
#include "SphereRenderer.h"

class directxProcess :public windowsApp
{
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

	DeferredRender mDeferredTech;
	CameraData mCamData;
	LightData mLightData;
	GCamera mCamera;
	bool mInit = false;
	SphereRenderer mSphereRenderer;

	std::shared_ptr<D3dDeviceManager> GetDeviceResources()
	{
		if (g_d3dObjects != nullptr && g_d3dObjects->IsDeviceRemoved())
		{
			// All references to the existing D3D device must be released before a new device
			// can be created.

			g_d3dObjects = nullptr;
			OnDeviceRemoved();
		}

		if (g_d3dObjects == nullptr)
		{
			g_d3dObjects = std::make_shared<D3dDeviceManager>();
			g_d3dObjects->SetWindows(mHwnd, mWidth, mHeight);
		}
		return g_d3dObjects;

	};

protected:
	void OnDeviceRemoved()
	{

	};
	void Setup()
	{
	
		
		auto commandQueue = GetDeviceResources()->GetCommandQueue();
		
		PIXBeginEvent(commandQueue, 0, L"Setup");
		{
			ThrowIfFailed(GetDeviceResources()->GetD3DDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_d3dObjects->GetCommandAllocator(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())));

			g_ShaderManager.LoadFolder("Shaders");

			
			mCamera.InitProjMatrix(3.14f*0.45f, mWidth, mHeight, 0.01f, 10000.0f);
			mCamera.Position(DirectX::XMFLOAT3(0,0.01f,-25.1f));
			mCamData.MVP = mCamera.ProjView();
			mCamData.InvPV = mCamera.InvScreenProjView();
			mCamData.CamPos = mCamera.Position();

			mLightData.pos = XMFLOAT3(0, 7, -15);

			mDeferredTech.Init();
			mDeferredTech.UpdateConstantBuffer(mCamData, mLightData);

			mSphereRenderer.Init(10, 20, 20);

			ThrowIfFailed(mCommandList->Close());
			ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
			GetDeviceResources()->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

			g_d3dObjects->WaitForGPU();
		}
		PIXEndEvent(commandQueue);
		mInit = true;
	}
	void Render()
	{
		
		auto commandQueue = GetDeviceResources()->GetCommandQueue();
		PIXBeginEvent(commandQueue, 0, L"Render");
		{
			ThrowIfFailed(g_d3dObjects->GetCommandAllocator()->Reset());
			ThrowIfFailed(mCommandList->Reset(g_d3dObjects->GetCommandAllocator(), mDeferredTech.getPSO()));
	
			
			AddResourceBarrier(mCommandList.Get(), g_d3dObjects->GetRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
			float clearColor[4]={ 0.0f,0.0f,0.0f,0.0f };
			mCommandList->ClearRenderTargetView(g_d3dObjects->GetRenderTargetView(), clearColor,0,nullptr);
			D3D12_RECT rect = { 0, 0, mWidth, mHeight };
			
			
		
			mCommandList->RSSetScissorRects(1, &rect);
			mCommandList->RSSetViewports(1, &g_d3dObjects->GetScreenViewport());

			mDeferredTech.ApplyGBufferPSO(mCommandList.Get());
			mSphereRenderer.Render(mCommandList);
		

			mCommandList->OMSetRenderTargets(1, &g_d3dObjects->GetRenderTargetView(),true, nullptr);
			mDeferredTech.ApplyLightingPSO(mCommandList.Get());

			AddResourceBarrier(mCommandList.Get(), g_d3dObjects->GetRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
			ThrowIfFailed(mCommandList->Close());
			ID3D12CommandList* ppCommandLists[1] = { mCommandList.Get() };

			commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

			g_d3dObjects->WaitForGPU();
		
			g_d3dObjects->Present();
			
		}
		PIXEndEvent(commandQueue);
	}

	void Update()
	{
		auto commandQueue = GetDeviceResources()->GetCommandQueue();
	
		PIXBeginEvent(commandQueue, 0, L"Update");
		{
			mCamera.Update();
			mCamData.MVP = mCamera.ProjView();
			mCamData.InvPV = mCamera.InvScreenProjView();
			mCamData.CamPos = mCamera.Position();
			mDeferredTech.UpdateConstantBuffer(mCamData, mLightData);
		}
		PIXEndEvent(commandQueue);

	}
	void Resize(UINT width, UINT height)
	{
		mWidth = width;
		mHeight = height;
		GetDeviceResources()->SetWindows(mHwnd,width,height);
		if (mInit)
		{
			mCamera.OnResize(mWidth, mHeight);
			mDeferredTech.InitWindowSizeDependentResources();
		}

	}

	void KeyDown(UINT key)
	{ 
		mCamera.KeyDown(key);
	return; }

    void MouseMove(UINT xpos, UINT ypos) {
		mCamera.InputMove(xpos, ypos);
		return; }
	void MousePress(UINT xpos, UINT ypos) {
		mCamera.InputPress(xpos,ypos);
		return; }


	
};



