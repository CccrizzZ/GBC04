//***************************************************************************************
// Week6-1-A3-Transparency.cpp 
//0 alpha means 0 % opaque, 0.4 means 40 % opaque, and 1.0 means 100 % opaque
//The relationship between opacity and transparency:
//T = 1− A, where A is opacity and T is transparency
//suppose that we want to blend the source and destination pixels based on the opacity of the source pixel 
//***************************************************************************************

#include "../../Common/d3dApp.h"
#include "../../Common/MathHelper.h"
#include "../../Common/UploadBuffer.h"
#include "../../Common/GeometryGenerator.h"
#include "../../Common/Camera.h"
#include "FrameResource.h"
#include "Waves.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")

const int gNumFrameResources = 3;

// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
struct RenderItem
{
	RenderItem() = default;

	// World matrix of the shape that describes the object's local space
	// relative to the world space, which defines the position, orientation,
	// and scale of the object in the world.
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

enum class RenderLayer : int
{
	Opaque = 0,
	Transparent,
	AlphaTested,
	AlphaTestedTreeSprites,
	Count
};

class A3 : public D3DApp
{
public:
	A3(HINSTANCE hInstance);
	A3(const A3& rhs) = delete;
	A3& operator=(const A3& rhs) = delete;
	~A3();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);
	void UpdateCamera(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMaterialCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);
	void UpdateWaves(const GameTimer& gt);

	void LoadTextures();
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildWavesGeometry();
	void BuildTreeSpritesGeometry();
	void BuildBoxGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

	float GetHillsHeight(float x, float z)const;
	XMFLOAT3 GetHillsNormal(float x, float z)const;

private:

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	UINT mCbvSrvDescriptorSize = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mTreeSpriteInputLayout;
	RenderItem* mWavesRitem = nullptr;

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

	std::unique_ptr<Waves> mWaves;

	PassConstants mMainPassCB;

	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f*XM_PI;
	float mPhi = XM_PIDIV2 - 0.1f;
	float mRadius = 50.0f;

	POINT mLastMousePos;
	Camera mCamera;

	
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		A3 theApp(hInstance);
		if (!theApp.Initialize())
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}

A3::A3(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

A3::~A3()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

bool A3::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	mCamera.SetPosition(0.0f, 10.0f, -60.0f);


	// Get the increment size of a descriptor in this heap type.  This is hardware specific, 
	// so we have to query this information.
	mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	mWaves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	LoadTextures();
	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	BuildWavesGeometry();
	BuildBoxGeometry();
	BuildTreeSpritesGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();

	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void A3::OnResize()
{
	D3DApp::OnResize();

	mCamera.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void A3::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);
	UpdateCamera(gt);

	// Cycle through the circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	AnimateMaterials(gt);
	UpdateObjectCBs(gt);
	UpdateMaterialCBs(gt);
	UpdateMainPassCB(gt);
	UpdateWaves(gt);
}

void A3::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), (float*)&mMainPassCB.FogColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);

	mCommandList->SetPipelineState(mPSOs["alphaTested"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::AlphaTested]);


	mCommandList->SetPipelineState(mPSOs["treeSprites"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::AlphaTestedTreeSprites]);

	mCommandList->SetPipelineState(mPSOs["transparent"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Transparent]);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
	mCurrFrameResource->Fence = ++mCurrentFence;


	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void A3::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void A3::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void A3::OnMouseMove(WPARAM btnState, int x, int y)
{

    if((btnState & MK_LBUTTON) != 0)
    {
		mCamera.Pitch(XMConvertToRadians(0.25f*float(y - mLastMousePos.y)));
		mCamera.RotateY(XMConvertToRadians(0.25f*float(x - mLastMousePos.x)));
    }

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void A3::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	
	if(GetAsyncKeyState('W') & 0x8000)
	{
		mCamera.Walk(0.1);

	}

	if(GetAsyncKeyState('S') & 0x8000)
	{
		mCamera.Walk(-0.1);

	}

	if(GetAsyncKeyState('A') & 0x8000)
	{
		mCamera.Strafe(-0.1);

	}

		if(GetAsyncKeyState('D') & 0x8000)
	{
		mCamera.Strafe(0.1);

	}
	

	mCamera.UpdateViewMatrix();

}

void A3::UpdateCamera(const GameTimer& gt)
{

}

void A3::AnimateMaterials(const GameTimer& gt)
{
	// Scroll the water material texture coordinates.
	auto waterMat = mMaterials["water"].get();

	float& tu = waterMat->MatTransform(3, 0);
	float& tv = waterMat->MatTransform(3, 1);

	tu += 0.1f * gt.DeltaTime();
	tv += 0.02f * gt.DeltaTime();

	if (tu >= 1.0f)
		tu -= 1.0f;

	if (tv >= 1.0f)
		tv -= 1.0f;

	waterMat->MatTransform(3, 0) = tu;
	waterMat->MatTransform(3, 1) = tv;

	// Material has changed, so need to update cbuffer.
	waterMat->NumFramesDirty = gNumFrameResources;
}

void A3::UpdateObjectCBs(const GameTimer& gt)
{
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& e : mAllRitems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void A3::UpdateMaterialCBs(const GameTimer& gt)
{
	auto currMaterialCB = mCurrFrameResource->MaterialCB.get();
	for (auto& e : mMaterials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}
}

void A3::UpdateMainPassCB(const GameTimer& gt)
{
	//XMMATRIX view = XMLoadFloat4x4(&mView);
	//XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX view = mCamera.GetView();
	XMMATRIX proj = mCamera.GetProj();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mCamera.GetPosition3f();
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.AmbientLight = { 0.5f, 0.5f, 0.5f, 0.8f };


	// directional light
	mMainPassCB.Lights[0].Position = { 0.0f, 10.0f, 0.0f };
	mMainPassCB.Lights[0].Direction = { 0, -1.0f, 0 };
	mMainPassCB.Lights[0].Strength = { 0.1f, 0.1f, 0.1f };


	// point lights
	mMainPassCB.Lights[1].Position = { 2.0f, 40.0f, -2.0f };
	mMainPassCB.Lights[1].Strength = { 10.0f, 1.0f, 1.0f };

	mMainPassCB.Lights[2].Position = { 4.0f, 40.0f, -2.0f };
	mMainPassCB.Lights[2].Strength = { 10.0f, 1.0f, 4.0f };

	// mMainPassCB.Lights[3].Position = { -36.0f, 17.0f, -5.0f };
	// mMainPassCB.Lights[3].Strength = { 6.0f, 2.0f, 10.0f };

	mMainPassCB.Lights[4].Position = { -13.0f, 10.0f, -10.0f };
	mMainPassCB.Lights[4].Strength = { 10.0f, 1.0f, 1.0f };

	mMainPassCB.Lights[5].Position = { -6.0f, 10.0f, 0.0f };
	mMainPassCB.Lights[5].Strength = { 3.0f, 1.0f, 10.0f };

	mMainPassCB.Lights[6].Position = { -2.0f, 18.0f, -5.0f };
	mMainPassCB.Lights[6].Strength = { 6.0f, 1.0f, 6.0f };

	mMainPassCB.Lights[7].Position = { 8.0f, 18.0f, -5.0f };
	mMainPassCB.Lights[7].Strength = { 8.0f, 1.0f, 1.0f };

	mMainPassCB.Lights[8].Position = { 16.0f, 15.0f, -5.0f };
	mMainPassCB.Lights[8].Strength = { 0.5f, 2.0f, 0.5f };

	mMainPassCB.Lights[9].Position = { -25.0f, 17.0f, -5.0f };
	mMainPassCB.Lights[9].Strength = { 4.0f, 1.0f, 10.0f };

	mMainPassCB.Lights[10].Position = { 35.0f, 4.5f, -16.5f };
	mMainPassCB.Lights[10].Strength = { 2.0f, 2.0f, 1.0f };

	mMainPassCB.Lights[11].Position = { -35.0f, 4.5f, -16.5f };
	mMainPassCB.Lights[11].Strength = { 2.0f, 2.0f, 1.0f };
	
	mMainPassCB.Lights[12].Position = { 35.0f, 4.5f, 16.5f };
	mMainPassCB.Lights[12].Strength = { 2.0f, 2.0f, 1.0f };

	mMainPassCB.Lights[13].Position = { -35.0f, 4.5f, 16.5f };
	mMainPassCB.Lights[13].Strength = { 2.0f, 2.0f, 1.0f };

	// spot lights
	mMainPassCB.Lights[14].Position = { 9.0f, 4.5f, 16.5f };
	mMainPassCB.Lights[14].Strength = { 3.0f, 3.0f, 1.0f };
	mMainPassCB.Lights[14].SpotPower = 1.0f;

	mMainPassCB.Lights[15].Position = { 9.0f, 4.5f, -16.5f };
	mMainPassCB.Lights[15].Strength = { 3.0f, 3.0f, 1.0f };
	mMainPassCB.Lights[15].SpotPower = 1.0f;

	mMainPassCB.Lights[3].Position = { 28.0f, 12.0f, -5.0f };
	mMainPassCB.Lights[3].Strength = { 1.0f, 1.0f, 3.0f };
	
	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void A3::UpdateWaves(const GameTimer& gt)
{
	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if ((mTimer.TotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = MathHelper::Rand(4, mWaves->RowCount() - 5);
		int j = MathHelper::Rand(4, mWaves->ColumnCount() - 5);

		float r = MathHelper::RandF(0.2f, 0.2f);

		mWaves->Disturb(i, j, r);
	}

	// Update the wave simulation.
	mWaves->Update(gt.DeltaTime());

	// Update the wave vertex buffer with the new solution.
	auto currWavesVB = mCurrFrameResource->WavesVB.get();
	for (int i = 0; i < mWaves->VertexCount(); ++i)
	{
		Vertex v;

		v.Pos = mWaves->Position(i);
		v.Normal = mWaves->Normal(i);

		// Derive tex-coords from position by 
		// mapping [-w/2,w/2] --> [0,1]
		v.TexC.x = 0.5f + v.Pos.x / mWaves->Width();
		v.TexC.y = 0.5f - v.Pos.z / mWaves->Depth();

		currWavesVB->CopyData(i, v);
	}

	// Set the dynamic VB of the wave renderitem to the current frame VB.
	mWavesRitem->Geo->VertexBufferGPU = currWavesVB->Resource();
}

void A3::LoadTextures()
{
	auto grassTex = std::make_unique<Texture>();
	grassTex->Name = "grassTex";
	grassTex->Filename = L"../../Textures/grass.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), grassTex->Filename.c_str(),
		grassTex->Resource, grassTex->UploadHeap));

	auto waterTex = std::make_unique<Texture>();
	waterTex->Name = "waterTex";
	waterTex->Filename = L"../../Textures/water1.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), waterTex->Filename.c_str(),
		waterTex->Resource, waterTex->UploadHeap));

	auto fenceTex = std::make_unique<Texture>();
	fenceTex->Name = "fenceTex";
	fenceTex->Filename = L"../../Textures/WireFence.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), fenceTex->Filename.c_str(),
		fenceTex->Resource, fenceTex->UploadHeap));

	auto bricksTex = std::make_unique<Texture>();
	bricksTex->Name = "bricksTex";
	bricksTex->Filename = L"./Tex/RB.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), bricksTex->Filename.c_str(),
		bricksTex->Resource, bricksTex->UploadHeap));

	auto stoneTex = std::make_unique<Texture>();
	stoneTex->Name = "stoneTex";
	stoneTex->Filename = L"../../Textures/bricks2.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), stoneTex->Filename.c_str(),
		stoneTex->Resource, stoneTex->UploadHeap));

	auto tileTex = std::make_unique<Texture>();
	tileTex->Name = "tileTex";
	tileTex->Filename = L"./Tex/DT.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), tileTex->Filename.c_str(),
		tileTex->Resource, tileTex->UploadHeap));

	auto snowTex = std::make_unique<Texture>();
	snowTex->Name = "snowTex";
	snowTex->Filename = L"./Tex/SNW.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), snowTex->Filename.c_str(),
		snowTex->Resource, snowTex->UploadHeap));

	auto checkboardTex = std::make_unique<Texture>();
	checkboardTex->Name = "checkTex";
	checkboardTex->Filename = L"../../Textures/checkboard.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), checkboardTex->Filename.c_str(),
		checkboardTex->Resource, checkboardTex->UploadHeap));


	auto treeArrayTex = std::make_unique<Texture>();
	treeArrayTex->Name = "treeArrayTex";
	treeArrayTex->Filename = L"./Tex/tree01S.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), treeArrayTex->Filename.c_str(),	
		treeArrayTex->Resource, treeArrayTex->UploadHeap));


	auto roadTex = std::make_unique<Texture>();
	roadTex->Name = "roadTex";
	roadTex->Filename = L"./Tex/road.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), roadTex->Filename.c_str(),	
		roadTex->Resource, roadTex->UploadHeap));


	auto rogersTex = std::make_unique<Texture>();
	rogersTex->Name = "rogersTex";
	rogersTex->Filename = L"./Tex/rogers.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), rogersTex->Filename.c_str(),	
		rogersTex->Resource, rogersTex->UploadHeap));

	auto TDtex = std::make_unique<Texture>();
	TDtex->Name = "TDtex";
	TDtex->Filename = L"./Tex/td1.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), TDtex->Filename.c_str(),	
		TDtex->Resource, TDtex->UploadHeap));


	auto BMOtex = std::make_unique<Texture>();
	BMOtex->Name = "BMOtex";
	BMOtex->Filename = L"./Tex/BMO.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), BMOtex->Filename.c_str(),	
		BMOtex->Resource, BMOtex->UploadHeap));

	auto roadTex2 = std::make_unique<Texture>();
	roadTex2->Name = "roadTex2";
	roadTex2->Filename = L"./Tex/road2.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), roadTex2->Filename.c_str(),
		roadTex2->Resource, roadTex2->UploadHeap));

	
	auto GBCtex = std::make_unique<Texture>();
	GBCtex->Name = "GBCtex";
	GBCtex->Filename = L"./Tex/gbc1.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), GBCtex->Filename.c_str(),
		GBCtex->Resource, GBCtex->UploadHeap));

	mTextures[grassTex->Name] = std::move(grassTex);
	mTextures[waterTex->Name] = std::move(waterTex);
	mTextures[fenceTex->Name] = std::move(fenceTex);
	mTextures[bricksTex->Name] = std::move(bricksTex);
	mTextures[stoneTex->Name] = std::move(stoneTex);
	mTextures[tileTex->Name] = std::move(tileTex);
	mTextures[snowTex->Name] = std::move(snowTex);
	mTextures[checkboardTex->Name] = std::move(checkboardTex);
	mTextures[treeArrayTex->Name] = std::move(treeArrayTex);
	mTextures[roadTex->Name] = std::move(roadTex);
	mTextures[rogersTex->Name] = std::move(rogersTex);
	mTextures[TDtex->Name] = std::move(TDtex);
	mTextures[BMOtex->Name] = std::move(BMOtex);
	mTextures[roadTex2->Name] = std::move(roadTex2);
	mTextures[GBCtex->Name] = std::move(GBCtex);







	
}

void A3::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[1].InitAsConstantBufferView(0);
	slotRootParameter[2].InitAsConstantBufferView(1);
	slotRootParameter[3].InitAsConstantBufferView(2);

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void A3::BuildDescriptorHeaps()
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 16;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));

	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	auto grassTex = mTextures["grassTex"]->Resource;
	auto waterTex = mTextures["waterTex"]->Resource;
	auto fenceTex = mTextures["fenceTex"]->Resource;
	auto bricksTex = mTextures["bricksTex"]->Resource;
	auto stoneTex = mTextures["stoneTex"]->Resource;
	auto tileTex = mTextures["tileTex"]->Resource;
	auto snowTex = mTextures["snowTex"]->Resource;
	auto checkTex = mTextures["checkTex"]->Resource;
	auto treeArrayTex = mTextures["treeArrayTex"]->Resource;
	auto roadTex = mTextures["roadTex"]->Resource;
	auto rogersTex = mTextures["rogersTex"]->Resource;
	auto TDtex = mTextures["TDtex"]->Resource;
	auto BMOtex = mTextures["BMOtex"]->Resource;
	auto roadTex2 = mTextures["roadTex2"]->Resource;
	auto GBCtex = mTextures["GBCtex"]->Resource;






	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = grassTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	md3dDevice->CreateShaderResourceView(grassTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = waterTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(waterTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = fenceTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(fenceTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = bricksTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(bricksTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = stoneTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(stoneTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = tileTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(tileTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = snowTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(snowTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = checkTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(checkTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	
	srvDesc.Format = treeArrayTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(treeArrayTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	
	srvDesc.Format = roadTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(roadTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	
	srvDesc.Format = rogersTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(rogersTex.Get(), &srvDesc, hDescriptor);
	
	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	
	srvDesc.Format = TDtex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(TDtex.Get(), &srvDesc, hDescriptor);


	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	
	srvDesc.Format = BMOtex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(BMOtex.Get(), &srvDesc, hDescriptor);	
	
	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	
	srvDesc.Format = roadTex2->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(roadTex2.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	
	srvDesc.Format = GBCtex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(GBCtex.Get(), &srvDesc, hDescriptor);
}

void A3::BuildShadersAndInputLayout()
{
	const D3D_SHADER_MACRO defines[] =
	{
		"FOG", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST",
		NULL, NULL
	};

	mShaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_0");
	mShaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", defines, "PS", "ps_5_0");
	mShaders["alphaTestedPS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", alphaTestDefines, "PS", "ps_5_0");

	mShaders["treeSpriteVS"] = d3dUtil::CompileShader(L"Shaders\\TreeSprite.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["treeSpriteGS"] = d3dUtil::CompileShader(L"Shaders\\TreeSprite.hlsl", nullptr, "GS", "gs_5_1");
	mShaders["treeSpritePS"] = d3dUtil::CompileShader(L"Shaders\\TreeSprite.hlsl", alphaTestDefines, "PS", "ps_5_1");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	mTreeSpriteInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void A3::BuildTreeSpritesGeometry()
{

	struct TreeSpriteVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Size;
	};

	static const int treeCount = 31;
	std::array<TreeSpriteVertex, 31> vertices;

	int temp = 0;
	for (int i = 1; i < 10; i++)
	{
		vertices[i].Pos = XMFLOAT3((temp-40),2,-25);
		vertices[i].Size = XMFLOAT2(5.0f, 5.0f);
		temp+=10;
	}
	
	temp = 0;
	for (int i = 11; i < 20; i++)
	{
		vertices[i].Pos = XMFLOAT3((temp-40),2,25);
		vertices[i].Size = XMFLOAT2(5.0f, 5.0f);
		temp+=10;
	}

	temp = 0;
	for (int i = 21; i < 26; i++)
	{
		vertices[i].Pos = XMFLOAT3(45, 2, (temp-20));
		vertices[i].Size = XMFLOAT2(5.0f, 5.0f);
		temp += 10;
	}

	temp = 0;
	for (int i = 26; i < 31; i++)
	{
		vertices[i].Pos = XMFLOAT3(-45, 2, (temp - 20));
		vertices[i].Size = XMFLOAT2(5.0f, 5.0f);
		temp += 10;
	}


	std::array<std::uint16_t, 31> indices =
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(TreeSpriteVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "treeSpritesGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(TreeSpriteVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["points"] = submesh;

	mGeometries["treeSpritesGeo"] = std::move(geo);
}

void A3::BuildWavesGeometry()
{
	std::vector<std::uint16_t> indices(3 * mWaves->TriangleCount()); // 3 indices per face
	assert(mWaves->VertexCount() < 0x0000ffff);

	// Iterate over each quad.
	int m = mWaves->RowCount();
	int n = mWaves->ColumnCount();
	int k = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1)*n + j;

			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}

	UINT vbByteSize = mWaves->VertexCount() * sizeof(Vertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "waterGeo";

	// Set dynamically.
	geo->VertexBufferCPU = nullptr;
	geo->VertexBufferGPU = nullptr;

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = submesh;

	mGeometries["waterGeo"] = std::move(geo);
}

void A3::BuildBoxGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
	GeometryGenerator::MeshData cone = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();
	UINT coneVertexOffset = cylinderVertexOffset + (UINT)cylinder.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();
	UINT coneIndexOffset = cylinderIndexOffset + (UINT)cylinder.Indices32.size();

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	SubmeshGeometry coneSubmesh;
	coneSubmesh.IndexCount = (UINT)cone.Indices32.size();
	coneSubmesh.StartIndexLocation = coneIndexOffset;
	coneSubmesh.BaseVertexLocation = coneVertexOffset;
	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size() +
		cone.Vertices.size();

	std::vector<Vertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].TexC = box.Vertices[i].TexC;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].TexC = grid.Vertices[i].TexC;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].TexC = sphere.Vertices[i].TexC;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].TexC = cylinder.Vertices[i].TexC;
	}

	for (size_t i = 0; i < cone.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cone.Vertices[i].Position;
		vertices[k].Normal = cone.Vertices[i].Normal;
		vertices[k].TexC = cone.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));
	indices.insert(indices.end(), std::begin(cone.GetIndices16()), std::end(cone.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;
	geo->DrawArgs["cone"] = coneSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void A3::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	opaquePsoDesc.pRootSignature = mRootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
		mShaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
		mShaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = mBackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));

	//
	// PSO for transparent objects
	//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = opaquePsoDesc;

	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	transparencyBlendDesc.BlendEnable = true;
	transparencyBlendDesc.LogicOpEnable = false;

	//suppose that we want to blend the sourceand destination pixels based on the opacity of the source pixel :
	//source blend factor : D3D12_BLEND_SRC_ALPHA
	//destination blend factor : D3D12_BLEND_INV_SRC_ALPHA
	//blend operator : D3D12_BLEND_OP_ADD


	transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//you could  specify the blend factor or not..
	//F = (r, g, b) and F = a, where the color (r, g, b,a) is supplied to the  parameter of the ID3D12GraphicsCommandList::OMSetBlendFactor method.
	//transparencyBlendDesc.SrcBlend = D3D12_BLEND_BLEND_FACTOR;
	//transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_BLEND_FACTOR;

	//Hooman: try different blend operators to see the blending effect
	//D3D12_BLEND_OP_ADD,
	//D3D12_BLEND_OP_SUBTRACT,
	//D3D12_BLEND_OP_REV_SUBTRACT,
	//D3D12_BLEND_OP_MIN,
	//D3D12_BLEND_OP_MAX

	transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD,

		transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_BLUE;
	//Direct3D supports rendering to up to eight render targets simultaneously.
	transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&mPSOs["transparent"])));

	//
	// PSO for alpha tested objects
	//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaTestedPsoDesc = opaquePsoDesc;
	alphaTestedPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["alphaTestedPS"]->GetBufferPointer()),
		mShaders["alphaTestedPS"]->GetBufferSize()
	};
	alphaTestedPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&alphaTestedPsoDesc, IID_PPV_ARGS(&mPSOs["alphaTested"])));

	//
	// PSO for tree sprites
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC treeSpritePsoDesc = opaquePsoDesc;
	treeSpritePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["treeSpriteVS"]->GetBufferPointer()),
		mShaders["treeSpriteVS"]->GetBufferSize()
	};
	treeSpritePsoDesc.GS =
	{
		reinterpret_cast<BYTE*>(mShaders["treeSpriteGS"]->GetBufferPointer()),
		mShaders["treeSpriteGS"]->GetBufferSize()
	};
	treeSpritePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["treeSpritePS"]->GetBufferPointer()),
		mShaders["treeSpritePS"]->GetBufferSize()
	};
	//step1
	treeSpritePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	treeSpritePsoDesc.InputLayout = { mTreeSpriteInputLayout.data(), (UINT)mTreeSpriteInputLayout.size() };
	treeSpritePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&treeSpritePsoDesc, IID_PPV_ARGS(&mPSOs["treeSprites"])));
}

void A3::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),
			1, (UINT)mAllRitems.size(), (UINT)mMaterials.size(), mWaves->VertexCount()));
	}
}

void A3::BuildMaterials()
{
	auto grass = std::make_unique<Material>();
	grass->Name = "grass";
	grass->MatCBIndex = 0;
	grass->DiffuseSrvHeapIndex = 0;
	grass->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	grass->Roughness = 0.125f;

	// This is not a good water material definition, but we do not have all the rendering
	// tools we need (transparency, environment reflection), so we fake it for now.
	auto water = std::make_unique<Material>();
	water->Name = "water";
	water->MatCBIndex = 1;
	water->DiffuseSrvHeapIndex = 1;
	water->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	water->Roughness = 0.0f;

	auto wirefence = std::make_unique<Material>();
	wirefence->Name = "wirefence";
	wirefence->MatCBIndex = 2;
	wirefence->DiffuseSrvHeapIndex = 2;
	wirefence->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	wirefence->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	wirefence->Roughness = 0.25f;


	auto bricks0 = std::make_unique<Material>();
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 3;
	bricks0->DiffuseSrvHeapIndex = 3;
	bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks0->Roughness = 0.1f;

	auto stone0 = std::make_unique<Material>();
	stone0->Name = "stone0";
	stone0->MatCBIndex = 4;
	stone0->DiffuseSrvHeapIndex = 4;
	stone0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	stone0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	stone0->Roughness = 0.1f;

	auto tile0 = std::make_unique<Material>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 5;
	tile0->DiffuseSrvHeapIndex = 5;
	tile0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile0->Roughness = 0.1f;

	auto snow0 = std::make_unique<Material>();
	snow0->Name = "snow0";
	snow0->MatCBIndex = 6;
	snow0->DiffuseSrvHeapIndex = 6;
	snow0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f);
	snow0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	snow0->Roughness = 0.1f;

	auto check0 = std::make_unique<Material>();
	check0->Name = "check0";
	check0->MatCBIndex = 7;
	check0->DiffuseSrvHeapIndex = 7;
	check0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	check0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	check0->Roughness = 0.1f;

	auto treeSprites = std::make_unique<Material>();
	treeSprites->Name = "treeSprites";
	treeSprites->MatCBIndex = 8;
	treeSprites->DiffuseSrvHeapIndex = 8;
	treeSprites->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	treeSprites->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	treeSprites->Roughness = 0.125f;

	auto roadSprites = std::make_unique<Material>();
	roadSprites->Name = "roadSprites";
	roadSprites->MatCBIndex = 9;
	roadSprites->DiffuseSrvHeapIndex = 9;
	roadSprites->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	roadSprites->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	roadSprites->Roughness = 0.5f;

	auto rogersSprites = std::make_unique<Material>();
	rogersSprites->Name = "rogersSprites";
	rogersSprites->MatCBIndex = 10;
	rogersSprites->DiffuseSrvHeapIndex = 10;
	rogersSprites->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	rogersSprites->FresnelR0 = XMFLOAT3(1.0f, 1.0f, 1.0f);
	rogersSprites->Roughness = 0.5f;

	auto TDsprites = std::make_unique<Material>();
	TDsprites->Name = "TDsprites";
	TDsprites->MatCBIndex = 11;
	TDsprites->DiffuseSrvHeapIndex = 11;
	TDsprites->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	TDsprites->FresnelR0 = XMFLOAT3(1.0f, 1.0f, 1.0f);
	TDsprites->Roughness = 0.5f;

	auto BMOsprites = std::make_unique<Material>();
	BMOsprites->Name = "BMOsprites";
	BMOsprites->MatCBIndex = 12;
	BMOsprites->DiffuseSrvHeapIndex = 12;
	BMOsprites->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	BMOsprites->FresnelR0 = XMFLOAT3(1.0f, 1.0f, 1.0f);
	BMOsprites->Roughness = 0.5f;	
	
	
	auto roadSprites2 = std::make_unique<Material>();
	roadSprites2->Name = "roadSprites2";
	roadSprites2->MatCBIndex = 13;
	roadSprites2->DiffuseSrvHeapIndex = 13;
	roadSprites2->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	roadSprites2->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	roadSprites2->Roughness = 0.5f;

	auto GBCSprite = std::make_unique<Material>();
	GBCSprite->Name = "GBCSprite";
	GBCSprite->MatCBIndex = 14;
	GBCSprite->DiffuseSrvHeapIndex = 14;
	GBCSprite->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GBCSprite->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	GBCSprite->Roughness = 0.5f;

	mMaterials["grass"] = std::move(grass);
	mMaterials["water"] = std::move(water);
	mMaterials["wirefence"] = std::move(wirefence);
	mMaterials["bricks0"] = std::move(bricks0);
	mMaterials["stone0"] = std::move(stone0);
	mMaterials["tile0"] = std::move(tile0);
	mMaterials["snow0"] = std::move(snow0);
	mMaterials["check0"] = std::move(check0);
	mMaterials["treeSprites"] = std::move(treeSprites);
	mMaterials["roadSprites"] = std::move(roadSprites);
	mMaterials["rogersSprites"] = std::move(rogersSprites);
	mMaterials["TDsprites"] = std::move(TDsprites);
	mMaterials["BMOsprites"] = std::move(BMOsprites);
	mMaterials["roadSprites2"] = std::move(roadSprites2);
	mMaterials["GBCSprite"] = std::move(GBCSprite);






	
}

void A3::BuildRenderItems()
{
	auto wavesRitem = std::make_unique<RenderItem>();
	wavesRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&wavesRitem->World, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, -2.0f, 0.0f));
	XMStoreFloat4x4(&wavesRitem->TexTransform, XMMatrixScaling(10.0f, 15.0f, 1.0f));
	wavesRitem->ObjCBIndex = 0;
	wavesRitem->Mat = mMaterials["water"].get();
	wavesRitem->Geo = mGeometries["waterGeo"].get();
	wavesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
	wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mWavesRitem = wavesRitem.get();

	mRitemLayer[(int)RenderLayer::Transparent].push_back(wavesRitem.get());

	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(6.0f, 16.0f, 6.0f) * XMMatrixTranslation(16.0f, 8.0f, 0.0f));
	XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(5.0f, 15.0f, 15.0f));
	boxRitem->ObjCBIndex = 1;
	boxRitem->Geo = mGeometries["shapeGeo"].get();
	boxRitem->Mat = mMaterials["check0"].get();
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::AlphaTested].push_back(boxRitem.get());


	auto CNt0 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&CNt0->World, XMMatrixScaling(6.0f, 14.0f, 6.0f) * XMMatrixTranslation(3.0f, 20.0f, 0.0f));
	XMStoreFloat4x4(&CNt0->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	CNt0->ObjCBIndex = 2;
	CNt0->Geo = mGeometries["shapeGeo"].get();
	CNt0->Mat = mMaterials["tile0"].get();
	CNt0->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	CNt0->IndexCount = CNt0->Geo->DrawArgs["cylinder"].IndexCount;
	CNt0->StartIndexLocation = CNt0->Geo->DrawArgs["cylinder"].StartIndexLocation;
	CNt0->BaseVertexLocation = CNt0->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(CNt0.get());
	mAllRitems.push_back(std::move(CNt0));

	auto gridRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&gridRitem->World, XMMatrixScaling(100.0f, 80.0f, 60.0f) * XMMatrixTranslation(0.0f, -40.0f, 0.0f));
	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(4.0f, 4.0f, 4.0f));
	gridRitem->ObjCBIndex = 3;
	gridRitem->Geo = mGeometries["shapeGeo"].get();
	gridRitem->Mat = mMaterials["grass"].get();
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["box"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["box"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(gridRitem.get());
	mAllRitems.push_back(std::move(gridRitem));

	auto CNt = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&CNt->World, XMMatrixScaling(12.0f, 8.0f, 12.0f) * XMMatrixTranslation(3.0f, 28.0f, 0.0f));
	XMStoreFloat4x4(&CNt->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	CNt->ObjCBIndex = 4;
	CNt->Geo = mGeometries["shapeGeo"].get();
	CNt->Mat = mMaterials["tile0"].get();
	CNt->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	CNt->IndexCount = CNt->Geo->DrawArgs["sphere"].IndexCount;
	CNt->StartIndexLocation = CNt->Geo->DrawArgs["sphere"].StartIndexLocation;
	CNt->BaseVertexLocation = CNt->Geo->DrawArgs["sphere"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(CNt.get());

	mAllRitems.push_back(std::move(CNt));

	auto boxRitem5 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem5->World, XMMatrixScaling(6.0f, 12.0f, 6.0f) * XMMatrixTranslation(28.0f, 5.0f, 0.0f));
	XMStoreFloat4x4(&boxRitem5->TexTransform, XMMatrixScaling(5.0f, 15.0f, 15.0f));
	boxRitem5->ObjCBIndex = 5;
	boxRitem5->Geo = mGeometries["shapeGeo"].get();
	boxRitem5->Mat = mMaterials["bricks0"].get();
	boxRitem5->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem5->IndexCount = boxRitem5->Geo->DrawArgs["box"].IndexCount;
	boxRitem5->StartIndexLocation = boxRitem5->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem5->BaseVertexLocation = boxRitem5->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(boxRitem5.get());

	mAllRitems.push_back(std::move(boxRitem5));

	auto Arena = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&Arena->World, XMMatrixScaling(20.0f, 3.0f, 20.0f) * XMMatrixTranslation(-12.0f, 3.0f, 0.0f));
	XMStoreFloat4x4(&Arena->TexTransform, XMMatrixScaling(5.0f, 5.0f, 5.0f));
	Arena->ObjCBIndex = 6;
	Arena->Geo = mGeometries["shapeGeo"].get();
	Arena->Mat = mMaterials["tile0"].get();
	Arena->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Arena->IndexCount = Arena->Geo->DrawArgs["cylinder"].IndexCount;
	Arena->StartIndexLocation = Arena->Geo->DrawArgs["cylinder"].StartIndexLocation;
	Arena->BaseVertexLocation = Arena->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(Arena.get());

	mAllRitems.push_back(std::move(Arena));

	auto Arena2 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&Arena2->World, XMMatrixScaling(12.5f, 6.0f, 12.5f) * XMMatrixTranslation(-12.0f, 6.0f, 0.0f));
	XMStoreFloat4x4(&Arena2->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	Arena2->ObjCBIndex = 7;
	Arena2->Geo = mGeometries["shapeGeo"].get();
	Arena2->Mat = mMaterials["snow0"].get();
	Arena2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Arena2->IndexCount = Arena2->Geo->DrawArgs["sphere"].IndexCount;
	Arena2->StartIndexLocation = Arena2->Geo->DrawArgs["sphere"].StartIndexLocation;
	Arena2->BaseVertexLocation = Arena2->Geo->DrawArgs["sphere"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(Arena2.get());

	mAllRitems.push_back(std::move(Arena2));

	auto boxRitem6 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem6->World, XMMatrixScaling(6.0f, 15.0f, 6.0f) * XMMatrixTranslation(-26.0f, 6.0f, 0.0f));
	XMStoreFloat4x4(&boxRitem6->TexTransform, XMMatrixScaling(10.0f, 10.0f, 10.0f));
	boxRitem6->ObjCBIndex = 8;
	boxRitem6->Geo = mGeometries["shapeGeo"].get();
	boxRitem6->Mat = mMaterials["check0"].get();
	boxRitem6->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem6->IndexCount = boxRitem6->Geo->DrawArgs["box"].IndexCount;
	boxRitem6->StartIndexLocation = boxRitem6->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem6->BaseVertexLocation = boxRitem6->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(boxRitem6.get());
	mAllRitems.push_back(std::move(boxRitem6));

	auto boxRitem7 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem7->World, XMMatrixScaling(5.0f, 12.0f, 6.0f) * XMMatrixTranslation(-35.0f, 6.0f, 0.0f));
	XMStoreFloat4x4(&boxRitem7->TexTransform, XMMatrixScaling(5.0f, 15.0f, 15.0f));
	boxRitem7->ObjCBIndex = 9;
	boxRitem7->Geo = mGeometries["shapeGeo"].get();
	boxRitem7->Mat = mMaterials["bricks0"].get();
	boxRitem7->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem7->IndexCount = boxRitem7->Geo->DrawArgs["box"].IndexCount;
	boxRitem7->StartIndexLocation = boxRitem7->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem7->BaseVertexLocation = boxRitem7->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(boxRitem7.get());
	mAllRitems.push_back(std::move(boxRitem7));

	// Floor
	auto gridRitem2 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&gridRitem2->World, XMMatrixScaling(80.0f, 2.0f, 40.0f) * XMMatrixTranslation(0.0f, -0.8f, 0.0f));
	XMStoreFloat4x4(&gridRitem2->TexTransform, XMMatrixScaling(15.0f, 5.0f, 4.0f));
	gridRitem2->ObjCBIndex = 10;
	gridRitem2->Geo = mGeometries["shapeGeo"].get();
	gridRitem2->Mat = mMaterials["stone0"].get();
	gridRitem2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem2->IndexCount = gridRitem2->Geo->DrawArgs["box"].IndexCount;
	gridRitem2->StartIndexLocation = gridRitem2->Geo->DrawArgs["box"].StartIndexLocation;
	gridRitem2->BaseVertexLocation = gridRitem2->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(gridRitem2.get());
	mAllRitems.push_back(std::move(gridRitem2));

	// Tree
	auto treeSpritesRitem = std::make_unique<RenderItem>();
	treeSpritesRitem->World = MathHelper::Identity4x4();
	treeSpritesRitem->ObjCBIndex = 11;
	treeSpritesRitem->Mat = mMaterials["treeSprites"].get();
	treeSpritesRitem->Geo = mGeometries["treeSpritesGeo"].get();
	treeSpritesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	treeSpritesRitem->IndexCount = treeSpritesRitem->Geo->DrawArgs["points"].IndexCount;
	treeSpritesRitem->StartIndexLocation = treeSpritesRitem->Geo->DrawArgs["points"].StartIndexLocation;
	treeSpritesRitem->BaseVertexLocation = treeSpritesRitem->Geo->DrawArgs["points"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::AlphaTestedTreeSprites].push_back(treeSpritesRitem.get());

	// Road
	auto RoadItem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&RoadItem->World, XMMatrixScaling(5.0f, 0.5f, 22.0f) * XMMatrixTranslation(9.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&RoadItem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	RoadItem->ObjCBIndex = 12;
	RoadItem->Geo = mGeometries["shapeGeo"].get();
	RoadItem->Mat = mMaterials["roadSprites"].get();
	RoadItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	RoadItem->IndexCount = RoadItem->Geo->DrawArgs["box"].IndexCount;
	RoadItem->StartIndexLocation = RoadItem->Geo->DrawArgs["box"].StartIndexLocation;
	RoadItem->BaseVertexLocation = RoadItem->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(RoadItem.get());
	mAllRitems.push_back(std::move(RoadItem));

	// Rogers logo
	auto RogersItem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&RogersItem->World, XMMatrixScaling(7.0f, 2.0f, 0.5f) * XMMatrixTranslation(-12.0f, 6.0f, -7.0f));
	XMStoreFloat4x4(&RogersItem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	RogersItem->ObjCBIndex = 13;
	RogersItem->Geo = mGeometries["shapeGeo"].get();
	RogersItem->Mat = mMaterials["rogersSprites"].get();
	RogersItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	RogersItem->IndexCount = RogersItem->Geo->DrawArgs["box"].IndexCount;
	RogersItem->StartIndexLocation = RogersItem->Geo->DrawArgs["box"].StartIndexLocation;
	RogersItem->BaseVertexLocation = RogersItem->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::AlphaTested].push_back(RogersItem.get());
	mAllRitems.push_back(std::move(RogersItem));

	// Rogers2
	auto RogersItem2 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&RogersItem2->World, XMMatrixScaling(8.5f, 2.5f, 0.4f) * XMMatrixTranslation(-12.0f, 6.0f, -7.0f));
	XMStoreFloat4x4(&RogersItem2->TexTransform, XMMatrixScaling(0.5f, 0.5f, 0.5f));
	RogersItem2->ObjCBIndex = 14;
	RogersItem2->Geo = mGeometries["shapeGeo"].get();
	RogersItem2->Mat = mMaterials["tile0"].get();
	RogersItem2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	RogersItem2->IndexCount = RogersItem2->Geo->DrawArgs["box"].IndexCount;
	RogersItem2->StartIndexLocation = RogersItem2->Geo->DrawArgs["box"].StartIndexLocation;
	RogersItem2->BaseVertexLocation = RogersItem2->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::AlphaTested].push_back(RogersItem2.get());
	mAllRitems.push_back(std::move(RogersItem2));

	// TD logo
	auto TDitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&TDitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(14.5f, 14.5f, -2.5f));
	XMStoreFloat4x4(&TDitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	TDitem->ObjCBIndex = 15;
	TDitem->Geo = mGeometries["shapeGeo"].get();
	TDitem->Mat = mMaterials["TDsprites"].get();
	TDitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	TDitem->IndexCount = TDitem->Geo->DrawArgs["box"].IndexCount;
	TDitem->StartIndexLocation = TDitem->Geo->DrawArgs["box"].StartIndexLocation;
	TDitem->BaseVertexLocation = TDitem->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::AlphaTested].push_back(TDitem.get());
	mAllRitems.push_back(std::move(TDitem));
	
	// BMO logo
	auto BMOitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&BMOitem->World, XMMatrixScaling(5.0f, 2.0f, 1.5f)* XMMatrixTranslation(-26.0f, 12.0f, -2.5f));
	XMStoreFloat4x4(&BMOitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	BMOitem->ObjCBIndex = 16;
	BMOitem->Geo = mGeometries["shapeGeo"].get();
	BMOitem->Mat = mMaterials["BMOsprites"].get();
	BMOitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	BMOitem->IndexCount = BMOitem->Geo->DrawArgs["box"].IndexCount;
	BMOitem->StartIndexLocation = BMOitem->Geo->DrawArgs["box"].StartIndexLocation;
	BMOitem->BaseVertexLocation = BMOitem->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::AlphaTested].push_back(BMOitem.get());
	mAllRitems.push_back(std::move(BMOitem));


	// Road
	auto RoadItem2 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&RoadItem2->World, XMMatrixScaling(73.0f, 0.5f, 5.0f)* XMMatrixTranslation(1.0f, 0.0f, 13.5f));
	XMStoreFloat4x4(&RoadItem2->TexTransform, XMMatrixScaling(2.0f, 1.0f, 2.0f));
	RoadItem2->ObjCBIndex = 17;
	RoadItem2->Geo = mGeometries["shapeGeo"].get();
	RoadItem2->Mat = mMaterials["roadSprites2"].get();
	RoadItem2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	RoadItem2->IndexCount = RoadItem2->Geo->DrawArgs["box"].IndexCount;
	RoadItem2->StartIndexLocation = RoadItem2->Geo->DrawArgs["box"].StartIndexLocation;
	RoadItem2->BaseVertexLocation = RoadItem2->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(RoadItem2.get());
	mAllRitems.push_back(std::move(RoadItem2));

	auto RoadItem3 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&RoadItem3->World, XMMatrixScaling(73.0f, 0.5f, 5.0f)* XMMatrixTranslation(1.0f, 0.0f, -13.5f));
	XMStoreFloat4x4(&RoadItem3->TexTransform, XMMatrixScaling(2.0f, 1.0f, 2.0f));
	RoadItem3->ObjCBIndex = 18;
	RoadItem3->Geo = mGeometries["shapeGeo"].get();
	RoadItem3->Mat = mMaterials["roadSprites2"].get();
	RoadItem3->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	RoadItem3->IndexCount = RoadItem3->Geo->DrawArgs["box"].IndexCount;
	RoadItem3->StartIndexLocation = RoadItem3->Geo->DrawArgs["box"].StartIndexLocation;
	RoadItem3->BaseVertexLocation = RoadItem3->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(RoadItem3.get());
	mAllRitems.push_back(std::move(RoadItem3));


	auto RoadItem4 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&RoadItem4->World, XMMatrixScaling(5.0f, 0.5f, 22.0f)* XMMatrixTranslation(35.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&RoadItem4->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	RoadItem4->ObjCBIndex = 19;
	RoadItem4->Geo = mGeometries["shapeGeo"].get();
	RoadItem4->Mat = mMaterials["roadSprites"].get();
	RoadItem4->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	RoadItem4->IndexCount = RoadItem4->Geo->DrawArgs["box"].IndexCount;
	RoadItem4->StartIndexLocation = RoadItem4->Geo->DrawArgs["box"].StartIndexLocation;
	RoadItem4->BaseVertexLocation = RoadItem4->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(RoadItem4.get());
	mAllRitems.push_back(std::move(RoadItem4));


	auto pole = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&pole->World, XMMatrixScaling(0.5f, 3.0f, 0.5f)* XMMatrixTranslation(9.0f, 0.0f, 17.0f));
	XMStoreFloat4x4(&pole->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	pole->ObjCBIndex = 20;
	pole->Geo = mGeometries["shapeGeo"].get();
	pole->Mat = mMaterials["tile0"].get();
	pole->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pole->IndexCount = pole->Geo->DrawArgs["cylinder"].IndexCount;
	pole->StartIndexLocation = pole->Geo->DrawArgs["cylinder"].StartIndexLocation;
	pole->BaseVertexLocation = pole->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(pole.get());
	mAllRitems.push_back(std::move(pole));

	auto poleComponent = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&poleComponent->World, XMMatrixScaling(0.5f, 0.1f, 1.5f)* XMMatrixTranslation(9.0f, 4.5f, 16.5f));
	XMStoreFloat4x4(&poleComponent->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	poleComponent->ObjCBIndex = 21;
	poleComponent->Geo = mGeometries["shapeGeo"].get();
	poleComponent->Mat = mMaterials["tile0"].get();
	poleComponent->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	poleComponent->IndexCount = poleComponent->Geo->DrawArgs["box"].IndexCount;
	poleComponent->StartIndexLocation = poleComponent->Geo->DrawArgs["box"].StartIndexLocation;
	poleComponent->BaseVertexLocation = poleComponent->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(poleComponent.get());
	mAllRitems.push_back(std::move(poleComponent));
	



	auto pole2 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&pole2->World, XMMatrixScaling(0.5f, 3.0f, 0.5f)* XMMatrixTranslation(35.0f, 0.0f, -17.0f));
	XMStoreFloat4x4(&pole2->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	pole2->ObjCBIndex = 22;
	pole2->Geo = mGeometries["shapeGeo"].get();
	pole2->Mat = mMaterials["tile0"].get();
	pole2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pole2->IndexCount = pole2->Geo->DrawArgs["cylinder"].IndexCount;
	pole2->StartIndexLocation = pole2->Geo->DrawArgs["cylinder"].StartIndexLocation;
	pole2->BaseVertexLocation = pole2->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(pole2.get());
	mAllRitems.push_back(std::move(pole2));

	auto poleComponent2 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&poleComponent2->World, XMMatrixScaling(0.5f, 0.1f, 1.5f)* XMMatrixTranslation(35.0f, 4.5f, -16.5f));
	XMStoreFloat4x4(&poleComponent2->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	poleComponent2->ObjCBIndex = 23;
	poleComponent2->Geo = mGeometries["shapeGeo"].get();
	poleComponent2->Mat = mMaterials["tile0"].get();
	poleComponent2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	poleComponent2->IndexCount = poleComponent2->Geo->DrawArgs["box"].IndexCount;
	poleComponent2->StartIndexLocation = poleComponent2->Geo->DrawArgs["box"].StartIndexLocation;
	poleComponent2->BaseVertexLocation = poleComponent2->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(poleComponent2.get());
	mAllRitems.push_back(std::move(poleComponent2));

	


	auto pole3 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&pole3->World, XMMatrixScaling(0.5f, 3.0f, 0.5f)* XMMatrixTranslation(35.0f, 0.0f, 17.0f));
	XMStoreFloat4x4(&pole3->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	pole3->ObjCBIndex = 24;
	pole3->Geo = mGeometries["shapeGeo"].get();
	pole3->Mat = mMaterials["tile0"].get();
	pole3->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pole3->IndexCount = pole3->Geo->DrawArgs["cylinder"].IndexCount;
	pole3->StartIndexLocation = pole3->Geo->DrawArgs["cylinder"].StartIndexLocation;
	pole3->BaseVertexLocation = pole3->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(pole3.get());
	mAllRitems.push_back(std::move(pole3));

	auto poleComponent3 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&poleComponent3->World, XMMatrixScaling(0.5f, 0.1f, 1.5f)* XMMatrixTranslation(35.0f, 4.5f, 16.5f));
	XMStoreFloat4x4(&poleComponent3->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	poleComponent3->ObjCBIndex = 25;
	poleComponent3->Geo = mGeometries["shapeGeo"].get();
	poleComponent3->Mat = mMaterials["tile0"].get();
	poleComponent3->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	poleComponent3->IndexCount = poleComponent3->Geo->DrawArgs["box"].IndexCount;
	poleComponent3->StartIndexLocation = poleComponent3->Geo->DrawArgs["box"].StartIndexLocation;
	poleComponent3->BaseVertexLocation = poleComponent3->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(poleComponent3.get());
	mAllRitems.push_back(std::move(poleComponent3));

	auto pole4 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&pole4->World, XMMatrixScaling(0.5f, 3.0f, 0.5f)* XMMatrixTranslation(9.0f, 0.0f, -17.0f ));
	XMStoreFloat4x4(&pole4->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	pole4->ObjCBIndex = 26;
	pole4->Geo = mGeometries["shapeGeo"].get();
	pole4->Mat = mMaterials["tile0"].get();
	pole4->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pole4->IndexCount = pole4->Geo->DrawArgs["cylinder"].IndexCount;
	pole4->StartIndexLocation = pole4->Geo->DrawArgs["cylinder"].StartIndexLocation;
	pole4->BaseVertexLocation = pole4->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(pole4.get());
	mAllRitems.push_back(std::move(pole4));

	auto poleComponent4 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&poleComponent4->World, XMMatrixScaling(0.5f, 0.1f, 1.5f)* XMMatrixTranslation(9.0f, 4.5f, -16.5f ));
	XMStoreFloat4x4(&poleComponent4->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	poleComponent4->ObjCBIndex = 27;
	poleComponent4->Geo = mGeometries["shapeGeo"].get();
	poleComponent4->Mat = mMaterials["tile0"].get();
	poleComponent4->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	poleComponent4->IndexCount = poleComponent4->Geo->DrawArgs["box"].IndexCount;
	poleComponent4->StartIndexLocation = poleComponent4->Geo->DrawArgs["box"].StartIndexLocation;
	poleComponent4->BaseVertexLocation = poleComponent4->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(poleComponent4.get());
	mAllRitems.push_back(std::move(poleComponent4));


	auto pole5 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&pole5->World, XMMatrixScaling(0.5f, 3.0f, 0.5f)* XMMatrixTranslation(-35.0f, 0.0f, -17.0f));
	XMStoreFloat4x4(&pole5->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	pole5->ObjCBIndex = 28;
	pole5->Geo = mGeometries["shapeGeo"].get();
	pole5->Mat = mMaterials["tile0"].get();
	pole5->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pole5->IndexCount = pole5->Geo->DrawArgs["cylinder"].IndexCount;
	pole5->StartIndexLocation = pole5->Geo->DrawArgs["cylinder"].StartIndexLocation;
	pole5->BaseVertexLocation = pole5->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(pole5.get());
	mAllRitems.push_back(std::move(pole5));

	auto poleComponent6 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&poleComponent6->World, XMMatrixScaling(0.5f, 0.1f, 1.5f)* XMMatrixTranslation(-35.0f, 4.5f, -16.5f));
	XMStoreFloat4x4(&poleComponent6->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	poleComponent6->ObjCBIndex = 29;
	poleComponent6->Geo = mGeometries["shapeGeo"].get();
	poleComponent6->Mat = mMaterials["tile0"].get();
	poleComponent6->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	poleComponent6->IndexCount = poleComponent6->Geo->DrawArgs["box"].IndexCount;
	poleComponent6->StartIndexLocation = poleComponent6->Geo->DrawArgs["box"].StartIndexLocation;
	poleComponent6->BaseVertexLocation = poleComponent6->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(poleComponent6.get());
	mAllRitems.push_back(std::move(poleComponent6));




	auto pole6 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&pole6->World, XMMatrixScaling(0.5f, 3.0f, 0.5f)* XMMatrixTranslation(-35.0f, 0.0f, 17.0f));
	XMStoreFloat4x4(&pole6->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	pole6->ObjCBIndex = 30;
	pole6->Geo = mGeometries["shapeGeo"].get();
	pole6->Mat = mMaterials["tile0"].get();
	pole6->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pole6->IndexCount = pole6->Geo->DrawArgs["cylinder"].IndexCount;
	pole6->StartIndexLocation = pole6->Geo->DrawArgs["cylinder"].StartIndexLocation;
	pole6->BaseVertexLocation = pole6->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(pole6.get());
	mAllRitems.push_back(std::move(pole6));

	auto poleComponent5 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&poleComponent5->World, XMMatrixScaling(0.5f, 0.1f, 1.5f)* XMMatrixTranslation(-35.0f, 4.5f, 16.5f));
	XMStoreFloat4x4(&poleComponent5->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	poleComponent5->ObjCBIndex = 31;
	poleComponent5->Geo = mGeometries["shapeGeo"].get();
	poleComponent5->Mat = mMaterials["tile0"].get();
	poleComponent5->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	poleComponent5->IndexCount = poleComponent5->Geo->DrawArgs["box"].IndexCount;
	poleComponent5->StartIndexLocation = poleComponent5->Geo->DrawArgs["box"].StartIndexLocation;
	poleComponent5->BaseVertexLocation = poleComponent5->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::Opaque].push_back(poleComponent5.get());
	mAllRitems.push_back(std::move(poleComponent5));

	// GBC logo
	auto GBCitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&GBCitem->World, XMMatrixScaling(4.0f, 2.0f, 1.5f)* XMMatrixTranslation(27.5f, 9.5f, -2.5f));
	XMStoreFloat4x4(&GBCitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	GBCitem->ObjCBIndex = 32;
	GBCitem->Geo = mGeometries["shapeGeo"].get();
	GBCitem->Mat = mMaterials["GBCSprite"].get();
	GBCitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	GBCitem->IndexCount = GBCitem->Geo->DrawArgs["box"].IndexCount;
	GBCitem->StartIndexLocation = GBCitem->Geo->DrawArgs["box"].StartIndexLocation;
	GBCitem->BaseVertexLocation = GBCitem->Geo->DrawArgs["box"].BaseVertexLocation;
	mRitemLayer[(int)RenderLayer::AlphaTested].push_back(GBCitem.get());
	mAllRitems.push_back(std::move(GBCitem));

	mAllRitems.push_back(std::move(wavesRitem));
	mAllRitems.push_back(std::move(boxRitem));
	mAllRitems.push_back(std::move(treeSpritesRitem));

}

void A3::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();
	auto matCB = mCurrFrameResource->MaterialCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(
			mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
		);

		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex*objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex*matCBByteSize;

		cmdList->SetGraphicsRootDescriptorTable(0, tex);
		cmdList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> A3::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

float A3::GetHillsHeight(float x, float z)const
{
	return 0.3f*(z*sinf(0.1f*x) + x * cosf(0.1f*z));
}

XMFLOAT3 A3::GetHillsNormal(float x, float z)const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}
