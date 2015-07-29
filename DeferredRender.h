#include"DirectxHelper.h"
#include "ShaderManager.h"
#include "ScreenQuadRenderer.h"
#include "d3dx12.h"

//To do: loading texture support
//1.Improve G-buffer shader
//2.Descriptor support

struct CameraData{
	DirectX::XMFLOAT4X4 MVP;
	DirectX::XMFLOAT4X4 InvPV;
	DirectX::XMFLOAT3 CamPos;
};
struct LightData {
	DirectX::XMFLOAT3 pos;

};
class DeferredRender
{
public:
	void Init();
	void InitWindowSizeDependentResources();
	ID3D12PipelineState* getPSO() { return m_pipelineState.Get(); }
	ID3D12RootSignature* getSignaturet() { return m_rootSignature.Get(); }
	void ApplyGBufferPSO(ID3D12GraphicsCommandList* command,bool bSetPSO=false);
	void ApplyLightingPSO(ID3D12GraphicsCommandList* command, bool bSetPSO = false);

	void UpdateConstantBuffer(CameraData& camData,LightData& ligData);
private:
	void CreateLightPassPSO();
	void CreateRootSignature();
	void CreatePso();
	void CreateCB();
	void CreateViews();
	void CreateDSV();
	void CreateRTV();

	void CreateSRV();
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12PipelineState> mLightPso;
	ComPtr<ID3D12DescriptorHeap> mCbvSrvUavDescriptorHeap;
	ScreenQuadRenderer mQuadRender;

	D3D12_GPU_DESCRIPTOR_HANDLE mGpuHandle;

	//0: CBV-Camera data
	//1: CBV-Light data
	//2: SRV-Albedo(RTV->SRV)
	//3: SRV-Normal(RTV->SRV)
	//4: SRV-Specular&gloss(RTV->SRV)
	//5: SRV-Depth(DSV->SRV)
	CDescriptorHeapWrapper m_cbvsrvHeap;

	CDescriptorHeapWrapper m_dsvHeap;
	CDescriptorHeapWrapper m_rtvHeap;
	CDescriptorHeapWrapper m_srvHeap;

	ComPtr<ID3D12Resource> mViewCB;
	ComPtr<ID3D12Resource> mLightCB;
	ComPtr<ID3D12Resource> mDsTexture;

	float mClearColor[4] = { 0.0,0.0f,0.0f,1.0f };
	float mClearDepth = 1.0f;
	const static int numRTV = 3;
	ComPtr<ID3D12Resource> mRtvTexture[numRTV];

	DXGI_FORMAT mDsvFormat= DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT mRtvFormat[3] = { DXGI_FORMAT_R11G11B10_FLOAT,DXGI_FORMAT_R8G8B8A8_SNORM,DXGI_FORMAT_R8G8B8A8_UNORM };
	struct ConstantBuffer
	{
		DirectX::XMMATRIX mat;
		float color;
	};
};