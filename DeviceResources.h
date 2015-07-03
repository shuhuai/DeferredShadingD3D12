
#include "stdafx.h"
#include <comdef.h>
#include "d3dx12.h"
#pragma once



namespace DX
{
	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
	}
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw _com_error(hr);
		}
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
			offsetted = ptr + index * HandleIncrementSize;
			return offsetted;
		}
		UINT64 MakeOffsetted(UINT64 ptr, UINT index)
		{
			UINT64 offsetted;
			offsetted = ptr + index * HandleIncrementSize;
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

	// Controls all the DirectX device resources.
	class DeviceResources
	{
	public:
		DeviceResources();
		void SetWindow(HWND hwnd,UINT width,UINT height);
		void SetLogicalSize(UINT width, UINT height);

	
		void ValidateDevice();
		void Present();
		void WaitForGpu();

		
		bool						IsDeviceRemoved() const				{ return m_deviceRemoved; }

		// D3D Accessors.
		ID3D12Device*				GetD3DDevice() const				{ return m_d3dDevice.Get(); }
		IDXGISwapChain*				GetSwapChain() const				{ return m_swapChain.Get(); }
		ID3D12Resource*				GetRenderTarget() const				{ return m_renderTargets[m_currentFrame].Get(); }
		ID3D12CommandQueue*			GetCommandQueue() const				{ return m_commandQueue.Get(); }
		ID3D12CommandAllocator*		GetCommandAllocator() const			{ return m_commandAllocators[m_currentFrame].Get(); }
		D3D12_VIEWPORT				GetScreenViewport() const			{ return m_screenViewport; }
		DirectX::XMFLOAT4X4			GetOrientationTransform3D() const	{ return m_orientationTransform3D; }

	

	private:
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void MoveToNextFrame();
	

		static const UINT								c_frameCount = 3;	// Use triple buffering.
		UINT											m_currentFrame;

		// Direct3D objects.
		Microsoft::WRL::ComPtr<ID3D12Device>			m_d3dDevice;
		Microsoft::WRL::ComPtr<IDXGIFactory4>			m_dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain1>			m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource>			m_renderTargets[c_frameCount];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	m_rtvHeap;
	
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>	m_commandAllocators[c_frameCount];
		D3D12_VIEWPORT									m_screenViewport;
		bool											m_deviceRemoved;

		// CPU/GPU Synchronization.
		Microsoft::WRL::ComPtr<ID3D12Fence>				m_fence;
		UINT64											m_fenceValues[c_frameCount];
		HANDLE											m_fenceEvent;

		// Cached reference to the Window.
		HWND m_hwnd;

		// Cached device properties.
		UINT					m_renderTargetWidth;
		UINT					m_renderTargetHeight;
		UINT					m_outputWidth;
		UINT					m_outputHeight;
		UINT					m_logicalWidth;
		UINT					m_logicalHeight;
		float											m_dpi;

		// Transforms used for display orientation.
		DirectX::XMFLOAT4X4 m_orientationTransform3D;
	};
}
