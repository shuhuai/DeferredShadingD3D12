
#include "stdafx.h"
#include <comdef.h>
//To do:
//Create a helper class to convert upload to default resources
#pragma once

using namespace Microsoft::WRL;

class D3dDeviceManager
{
	
public:
	D3dDeviceManager();
	~D3dDeviceManager();
	void CreateDeviceResources();
	void WaitForGPU();
	void CreateWindowSizeDependentResources();
	void SetWindows(HWND hwnd, UINT width, UINT height);
	void ValidateDevice();
	void Present();
	void MoveToNextFrame();
	bool						IsDeviceRemoved() const { return m_deviceRemoved; }
	// D3D Accessors.
	ID3D12Device*				GetD3DDevice() const { return m_d3dDevice.Get(); }
	IDXGISwapChain*				GetSwapChain() const { return m_swapChain.Get(); }
	ID3D12Resource*				GetRenderTarget() const { return m_renderTargets[m_currentFrame].Get(); }
	ID3D12CommandQueue*			GetCommandQueue() const { return m_commandQueue.Get(); }
	ID3D12CommandAllocator*		GetCommandAllocator() const { return m_commandAllocators[m_currentFrame].Get(); }
	D3D12_VIEWPORT				GetScreenViewport() const { return m_screenViewport; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const
	{	
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = m_rtvHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_rtvDescriptorSize*m_currentFrame;
		return handle;
	}
private:
	// Cached reference to the Window.
		HWND m_hwnd;
		// Direct3D objects.
		ComPtr<ID3D12Device> m_d3dDevice;
		ComPtr<IDXGISwapChain1> m_swapChain;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGIFactory4> m_dxgiFactory;
		WCHAR* m_sDeviceName;
		static const UINT								c_frameCount = 3;	// Use triple buffering.
		UINT											m_currentFrame;
		ComPtr<ID3D12CommandAllocator> m_commandAllocators[c_frameCount];
		ComPtr<ID3D12Resource>			m_renderTargets[c_frameCount];
		bool											m_deviceRemoved;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		UINT m_rtvDescriptorSize;
		D3D12_VIEWPORT									m_screenViewport;

		// CPU/GPU Synchronization.
		ComPtr<ID3D12Fence>				m_fence;
		UINT64											m_fenceValues[c_frameCount];
		HANDLE											m_fenceEvent;
		// Cached device properties.
		UINT					m_renderTargetWidth;
		UINT					m_renderTargetHeight;
		UINT					m_outputWidth;
		UINT					m_outputHeight;

		



};

extern std::shared_ptr<D3dDeviceManager> g_d3dObjects;

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw _com_error(hr);
		}
	}

static void AddResourceBarrier(
	ID3D12GraphicsCommandList* command,
	ID3D12Resource* pResource,
	D3D12_RESOURCE_STATES before,
	D3D12_RESOURCE_STATES after
	)
{
	D3D12_RESOURCE_BARRIER desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	desc.Transition.pResource = pResource;
	desc.Transition.StateBefore = before;
	desc.Transition.StateAfter = after;
	command->ResourceBarrier(1, &desc);
};

struct ShaderObject {
	void* binaryPtr;
	size_t  size;
	std::string name;
};

struct DescriptorHandleObject
{
	D3D12_CPU_DESCRIPTOR_HANDLE CPUHeapStart;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHeapStart;
};
static void SetVSShader(ShaderObject obj, D3D12_GRAPHICS_PIPELINE_STATE_DESC descPipelineState)
{
	descPipelineState.VS.pShaderBytecode = obj.binaryPtr;
	descPipelineState.VS.BytecodeLength = obj.size;
}
static void SetFSShader(ShaderObject obj, D3D12_GRAPHICS_PIPELINE_STATE_DESC descPipelineState)
{
	descPipelineState.PS.pShaderBytecode = obj.binaryPtr;
	descPipelineState.PS.BytecodeLength = obj.size;
}
class CDescriptorHeapWrapper
{
public:
	CDescriptorHeapWrapper() { memset(this, 0, sizeof(*this)); }

	HRESULT Create(
		ID3D12Device* pDevice,
		D3D12_DESCRIPTOR_HEAP_TYPE Type,
		UINT NumDescriptors,
		bool bShaderVisible = false)
	{
		Desc.Type = Type;
		Desc.NumDescriptors = NumDescriptors;
		Desc.Flags = (bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : (D3D12_DESCRIPTOR_HEAP_FLAGS)0);

		HRESULT hr = pDevice->CreateDescriptorHeap(&Desc,
			__uuidof(ID3D12DescriptorHeap),
			(void**)&pDH);
		if (FAILED(hr)) return hr;

		hCPUHeapStart = pDH->GetCPUDescriptorHandleForHeapStart();
		if (bShaderVisible)
		{
			hGPUHeapStart = pDH->GetGPUDescriptorHandleForHeapStart();
		}
		else
		{
			hGPUHeapStart.ptr = 0;
		}
		HandleIncrementSize = pDevice->GetDescriptorHandleIncrementSize(Desc.Type);
		return hr;
	}
	operator ID3D12DescriptorHeap*() { return pDH.Get(); }

	SIZE_T MakeOffsetted(SIZE_T ptr, UINT index)
	{
		SIZE_T offsetted;
		offsetted = ptr + static_cast<SIZE_T>(index * HandleIncrementSize);
		return offsetted;
	}

	UINT64 MakeOffsetted(UINT64 ptr, UINT index)
	{
		UINT64 offsetted;
		offsetted = ptr + static_cast<UINT64>(index * HandleIncrementSize);
		return offsetted;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE hCPU(UINT index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = MakeOffsetted(hCPUHeapStart.ptr, index);
		return handle;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE hGPU(UINT index)
	{
		assert(Desc.Flags&D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		D3D12_GPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = MakeOffsetted(hGPUHeapStart.ptr, index);
		return handle;
	}
	D3D12_DESCRIPTOR_HEAP_DESC Desc;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDH;
	D3D12_CPU_DESCRIPTOR_HANDLE hCPUHeapStart;
	D3D12_GPU_DESCRIPTOR_HANDLE hGPUHeapStart;
	UINT HandleIncrementSize;
};

const UINT PIX_EVENT_UNICODE_VERSION = 0;

inline void PIXBeginEvent(ID3D12CommandQueue* pCommandQueue, UINT64 /*metadata*/, PCWSTR pFormat)
{
	pCommandQueue->BeginEvent(PIX_EVENT_UNICODE_VERSION, pFormat, (wcslen(pFormat) + 1) * sizeof(pFormat[0]));
}

inline void PIXBeginEvent(ID3D12GraphicsCommandList* pCommandList, UINT64 /*metadata*/, PCWSTR pFormat)
{

	pCommandList->BeginEvent(PIX_EVENT_UNICODE_VERSION, pFormat, (wcslen(pFormat) + 1) * sizeof(pFormat[0]));
}
inline void PIXEndEvent(ID3D12CommandQueue* pCommandQueue)
{
	pCommandQueue->EndEvent();
}
inline void PIXEndEvent(ID3D12GraphicsCommandList* pCommandList)
{
	pCommandList->EndEvent();
}
