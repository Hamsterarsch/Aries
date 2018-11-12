#include "Error.h"
#include <objbase.h>
#include <iostream>
#include <DirectXMath.h>
#include "ForwardRenderer.h"
#include "DeviceResources.h"
#include "WinWindow.h"
#include <string>
#include "ObjMesh.h"
#include "SceneGraph.h"
#include "Actor.h"
#include "VertexBase.h"
#include "PixelBase.h"
#include "TextureTarget2D.h"
#include "RenderPass.h"
#include "PassMetadata.h"

#define PASS_LAMBDA [](ID3D11DeviceContext &Context, std::shared_ptr<FPassMetadata> &pMetadata)

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	try
	{
		if (CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) != S_OK)
		{
			throw(FError{ -1, "Com initialization failed.", __FILE__, __LINE__ });

		}

		if (!DirectX::XMVerifyCPUSupport())
		{
			throw(FError{ -1, "Direct X Math not supported", __FILE__, __LINE__ });

		}

		//auto hInstance = static_cast<HINSTANCE>(GetModuleHandle(nullptr));// "MODULES can be used in place of HINSTANCE"

	//App init
		FWinWindow MainWindow{ hInstance, &MainWindowProc, L"AriesWndClass" };
		MainWindow.Show(nCmdShow);
		MainWindow.Show(SW_SHOW);

		auto pDeviceResources = std::make_shared<FDeviceResources>(MainWindow.GetHandle());
		FForwardRenderer Renderer{ pDeviceResources };
	

	//Shader creation
		auto pStandardVertex = std::make_shared<FVertexBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/Vertex.hlsl").data());
		auto pLambertianPixel = std::make_shared<FPixelBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/Fragment.hlsl").data());
		auto pToTexturePixel = std::make_shared<FPixelBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/Fragment.hlsl").data(), "main");
		auto pProjectionVertex = std::make_shared<FVertexBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/PlanarReflection_VS.hlsl").data(), "main");
		auto pProjectionPixel = std::make_shared<FPixelBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/PlanarReflection_PS.hlsl").data(), "main");


	//Pass data creation
		auto pPassData = std::make_shared<FPassMetadata>();
		pPassData->m_vSamplerStates.emplace_back();

		//Sampler
		CD3D11_SAMPLER_DESC SamplerDesc{};
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamplerDesc.BorderColor[0] = 0;
		SamplerDesc.BorderColor[1] = 0;
		SamplerDesc.BorderColor[2] = 0;
		SamplerDesc.BorderColor[3] = 0;
		SamplerDesc.MinLOD = 0;
		SamplerDesc.MipLODBias = 0;
		SamplerDesc.MaxAnisotropy = 1;
		SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		
		auto hr = pDeviceResources->GetDevice()->CreateSamplerState(&SamplerDesc, pPassData->m_vSamplerStates.at(0).GetAddressOf());

		//Reflect buffer
		CD3D11_BUFFER_DESC BufferDesc{ sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER };

		ComPtr<ID3D11Buffer> pReflectBuffer;

		auto ViewMat = Renderer.GetViewMatrix();
		//ViewMat = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0, -1, 0)) * ViewMat;

		DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, -.5, -2.f, 0.f);
		DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, -.5, 0.0f, 0.f);
		DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);
		ViewMat = DirectX::XMMatrixLookAtRH(Eye, At, Up);

		//ViewMat.r[1].m128_f32[3] *= -1;		

		D3D11_SUBRESOURCE_DATA MatData{};
		MatData.pSysMem = &ViewMat;

		pPassData->m_vBuffers.emplace_back();
		hr = pDeviceResources->GetDevice()->CreateBuffer(&BufferDesc, &MatData, pPassData->m_vBuffers.at(0).GetAddressOf());

		//Texture target
		RECT WindowRect{};
		GetWindowRect(MainWindow.GetHandle(), &WindowRect);

		FTextureTarget2D ReflectionTexture{ *pDeviceResources->GetDevice(),  static_cast<float>(WindowRect.right), static_cast<float>(WindowRect.bottom) };
		pPassData->m_vRenderTargets.push_back(ReflectionTexture.GetViewTargetCom());
		pPassData->m_vResourceViews.push_back(ReflectionTexture.GetViewResourceCom());


	//Cluster creation
		auto StandardCluster = std::make_shared<FShaderCluster>();
		StandardCluster->m_pVertex = pStandardVertex;
		StandardCluster->m_pPixel = pLambertianPixel;

		auto ProjectionCluster = std::make_shared<FShaderCluster>();
		ProjectionCluster->m_pVertex = pProjectionVertex;
		ProjectionCluster->m_pPixel = pProjectionPixel;

		auto ToTextureCluster = std::make_shared<FShaderCluster>();
		ToTextureCluster->m_pVertex = pStandardVertex;
		ToTextureCluster->m_pPixel = pToTexturePixel;


	//Pass creation
		auto pStandardPass = std::make_shared<FRenderPass>();
		pStandardPass->m_pShaderCluster = StandardCluster;
		
		auto pToTexturePass = std::make_shared<FRenderPass>();
		pToTexturePass->m_pShaderCluster = ToTextureCluster;
		pToTexturePass->m_pPassMetadata = pPassData;
		pToTexturePass->m_fOnBegin = PASS_LAMBDA
		{
			if (pMetadata)
			{
				//Pass init
				float Color[4] = { 1,1,1,1 };
				Context.ClearRenderTargetView(pMetadata->m_vRenderTargets.at(0).Get(), Color);
				Context.OMSetRenderTargets(1, pMetadata->m_vRenderTargets.at(0).GetAddressOf(), nullptr);
				

			}

		};
				
		auto pProjectionPass = std::make_shared<FRenderPass>();
		pProjectionPass->m_pShaderCluster = ProjectionCluster;
		pProjectionPass->m_pPassMetadata = pPassData;
		pProjectionPass->m_fOnBegin = PASS_LAMBDA
		{
			//Pass init
			if (pMetadata)
			{
				Context.VSSetConstantBuffers(1, 1, pMetadata->m_vBuffers.at(0).GetAddressOf());
				Context.PSSetSamplers(0, 1, pMetadata->m_vSamplerStates.at(0).GetAddressOf());
				Context.PSSetShaderResources(0, 1, pMetadata->m_vResourceViews.at(0).GetAddressOf());

			}


		};
		pProjectionPass->m_fOnFinish = PASS_LAMBDA
		{
			ID3D11ShaderResourceView *NullView{ nullptr };
			Context.PSSetShaderResources(0, 1, &NullView);

		};
		

	//Scene population
		FSceneGraph SceneGraph{};
		//0
		SceneGraph.m_vActors.push_back(std::make_unique<FActor>(std::make_shared<FObjMesh>(FModulePath::MakeExeRelative("../../assets/Torus.obj").data(), *pDeviceResources)));

			//Pass setup
		SceneGraph.m_vActors.at(0)->GetRenderPasses() = { pStandardPass, pToTexturePass };

			//matrix transforms
		SceneGraph.m_vActors.at(0)->Translate(DirectX::XMVectorSet(0, 1.5, 0, 0));
		SceneGraph.m_vActors.at(0)->Scale(.5f, .5f, .5f);
		
		//1
		SceneGraph.m_vActors.push_back(std::make_unique<FActor>(std::make_shared<FObjMesh>(FModulePath::MakeExeRelative("../../assets/Plane.obj").data(), *pDeviceResources)));
			//Pass setup
		SceneGraph.m_vActors.at(1)->GetRenderPasses() = { pProjectionPass };
		

	//App start
		MSG Msg{};		
		while (Msg.message != WM_QUIT)
		{
			if (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
			{
				DispatchMessage(&Msg);
				
			}
			else
			{
				Renderer.Render(SceneGraph.m_vActors);
				pDeviceResources->PresentBackbuffer();

			}

		}

		CoUninitialize();

		return Msg.message;

	}
	catch (FError &Error)
	{
		std::string Str(Error.m_ErrorMessage);
		Str += " Error occured in ";
		Str += Error.m_CodeFilePath;
		Str += " at line ";
		Str += std::to_string(Error.m_ErrorLineNumber);
		
		auto BuffSize = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, Str.data(), Str.size(), nullptr, 0);
		auto Wstr = std::make_unique<wchar_t[]>(BuffSize);
		MultiByteToWideChar(CP_ACP, MB_COMPOSITE, Str.data(), Str.size(), Wstr.get(), BuffSize);
		
		MessageBox(nullptr, Wstr.get(), L"Error", MB_ICONERROR);
		return Error.m_ReturnValue;

	}
	catch (...)
	{

		return -1;
	}

	return 0;


}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_DESTROY:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);

	}
	return 0;


}

INT_PTR CALLBACK AboutProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		return true;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, LOWORD(wParam));
			return true;

		}
		break;

	}
	return false;


}
