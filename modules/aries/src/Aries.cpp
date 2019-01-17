#include "Error.h"
#include <objbase.h>
#include <iostream>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <Windows.h>
//#include "WinWindow.h"
#include <string>
#include "Rhi.h"
//#include "dx12/DX12Factory.h"


//#define PASS_LAMBDA [](ID3D11DeviceContext &Context, std::shared_ptr<FPassMetadata> &pMetadata)

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

		auto pFactory{ GetGAPIFactory(EAPIType::DX12) };
		auto pMainWnd = pFactory->MakeWindow(1920, 1080, L"MainWindow", &MainWindowProc, L"WndClass");

		auto pHeap{ pFactory->MakeHeap(EHeapType::Default, 256, EResourceCategory::Buffer, false) };
		auto pBufferTest{ pFactory->MakeReservedBuffer(EBufferTypes::Constant, 64) };

	/*
	//App init
		FWinWindow MainWindow{ hInstance, &MainWindowProc, L"AriesWndClass" };
		MainWindow.Show(nCmdShow);
		MainWindow.Show(SW_SHOW);

		auto pDeviceResources = std::make_shared<FDeviceResources>(MainWindow.GetHandle());
		FForwardRenderer Renderer{ pDeviceResources };
	

	//Shader creation
		auto pStandardVertex = std::make_shared<FVertexBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/Vertex.hlsl").data());
		auto pLambertianPixel = std::make_shared<FPixelBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/Fragment.hlsl").data());
		auto pToTextureVertex = std::make_shared<FVertexBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/ViewToTexture_VS.hlsl").data());
		auto pProjectionVertex = std::make_shared<FVertexBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/PlanarReflection_VS.hlsl").data(), "main");
		auto pProjectionPixel = std::make_shared<FPixelBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/PlanarReflection_PS.hlsl").data(), "main");
		auto pShadowMapVertex = std::make_shared<FVertexBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/SingleLightMapping_VS.hlsl").data());
		auto pShadowMapPixel = std::make_shared<FPixelBase>(pDeviceResources->GetDevice(), FModulePath::MakeExeRelative(L"../../shaders/SingleLightMapping_PS.hlsl").data());

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

		auto ZeroPlaneReflect = Renderer.GetViewMatrix();;
		ZeroPlaneReflect.r[3].m128_f32[1] *= -1;

		D3D11_SUBRESOURCE_DATA MatData{};
		MatData.pSysMem = &ZeroPlaneReflect;

		pPassData->m_vBuffers.emplace_back();
		hr = pDeviceResources->GetDevice()->CreateBuffer(&BufferDesc, &MatData, pPassData->m_vBuffers.at(0).GetAddressOf());

		//Texture target
		RECT WindowRect{};
		GetWindowRect(MainWindow.GetHandle(), &WindowRect);

		FTextureTarget2D ReflectionTexture{ *pDeviceResources->GetDevice(),  static_cast<float>(WindowRect.right), static_cast<float>(WindowRect.bottom) };
		pPassData->m_vRenderTargets.push_back(ReflectionTexture.GetViewTargetCom());
		pPassData->m_vResourceViews.push_back(ReflectionTexture.GetViewResourceCom());


	//Cluster creation
		auto pStandardCluster = std::make_shared<FShaderCluster>();
		pStandardCluster->m_pVertex = pStandardVertex;
		pStandardCluster->m_pPixel = pLambertianPixel;

		auto pProjectionCluster = std::make_shared<FShaderCluster>();
		pProjectionCluster->m_pVertex = pProjectionVertex;
		pProjectionCluster->m_pPixel = pProjectionPixel;

		auto pToTextureCluster = std::make_shared<FShaderCluster>();
		pToTextureCluster->m_pVertex = pToTextureVertex;
		pToTextureCluster->m_pPixel = pLambertianPixel;

		auto pShadowMapCluster = std::make_shared<FShaderCluster>();
		pShadowMapCluster->m_pVertex = pShadowMapVertex;
		pShadowMapCluster->m_pPixel = pShadowMapPixel;


	//Pass creation
		auto pShadowMapMetadata = std::make_shared<FPassMetadata>();
		pShadowMapMetadata->m_vBuffers.push_back(Renderer.m_SingleLightView.GetCom());
		pShadowMapMetadata->m_vResourceViews.push_back(pDeviceResources->GetDepthPrePassBuffer()->GetViewResourceCom());

		//Sampler
		CD3D11_SAMPLER_DESC DepthSampler{};
		DepthSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		DepthSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		DepthSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		DepthSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		DepthSampler.BorderColor[0] = 1;
		DepthSampler.BorderColor[1] = 1;
		DepthSampler.BorderColor[2] = 1;
		DepthSampler.BorderColor[3] = 1;
		DepthSampler.MinLOD = 0;
		DepthSampler.MipLODBias = 0;
		DepthSampler.MaxAnisotropy = 1;
		DepthSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		pShadowMapMetadata->m_vSamplerStates.emplace_back();
		auto Result = pDeviceResources->GetDevice()->CreateSamplerState(&DepthSampler, pShadowMapMetadata->m_vSamplerStates.at(0).GetAddressOf());

		auto pShadowMapPass = std::make_shared<FRenderPass>();
		pShadowMapPass->m_pShaderCluster = pShadowMapCluster;
		pShadowMapPass->m_pPassMetadata = pShadowMapMetadata;
		pShadowMapPass->m_fOnBegin = PASS_LAMBDA
		{
			if (pMetadata)
			{
				Context.VSSetConstantBuffers(1, 1, pMetadata->m_vBuffers.at(0).GetAddressOf());
				Context.PSSetConstantBuffers(1, 1, pMetadata->m_vBuffers.at(0).GetAddressOf());
				Context.PSSetShaderResources(0, 1, pMetadata->m_vResourceViews.at(0).GetAddressOf());
				Context.PSSetSamplers(0, 1, pMetadata->m_vSamplerStates.at(0).GetAddressOf());


			}

		};

		auto pStandardPass = std::make_shared<FRenderPass>();
		pStandardPass->m_pShaderCluster = pStandardCluster;
		
		auto pToTexturePass = std::make_shared<FRenderPass>();
		pToTexturePass->m_pShaderCluster = pToTextureCluster;
		pToTexturePass->m_pPassMetadata = pPassData;
		pToTexturePass->m_fOnBegin = PASS_LAMBDA
		{
			if (pMetadata)
			{
				//Pass init
				float Color[4] = { 1,1,1,1 };

				//bind reflection texture as target
				Context.ClearRenderTargetView(pMetadata->m_vRenderTargets.at(0).Get(), Color);
				Context.OMSetRenderTargets(1, pMetadata->m_vRenderTargets.at(0).GetAddressOf(), nullptr);

				//Set reflection matrix for vs
				Context.VSSetConstantBuffers(1, 1, pMetadata->m_vBuffers.at(0).GetAddressOf());

			}

		};
				
		auto pProjectReflectionPass = std::make_shared<FRenderPass>();
		pProjectReflectionPass->m_pShaderCluster = pProjectionCluster;
		pProjectReflectionPass->m_pPassMetadata = pPassData;
		pProjectReflectionPass->m_fOnBegin = PASS_LAMBDA
		{
			//Pass init
			if (pMetadata)
			{
				//Set reflection matrix for vs
				Context.VSSetConstantBuffers(1, 1, pMetadata->m_vBuffers.at(0).GetAddressOf());

				//Set texture resources for ps (sampling the reflection texture)
				Context.PSSetSamplers(0, 1, pMetadata->m_vSamplerStates.at(0).GetAddressOf());
				Context.PSSetShaderResources(0, 1, pMetadata->m_vResourceViews.at(0).GetAddressOf());

			}


		};

		pProjectReflectionPass->m_fOnFinish = PASS_LAMBDA
		{
			//Unbind the reflection texture
			ID3D11ShaderResourceView *NullView{ nullptr };
			Context.PSSetShaderResources(0, 1, &NullView);

		};
				

	//Scene population
		FSceneGraph SceneGraph{};
		//0
		SceneGraph.m_vActors.push_back(std::make_unique<FActor>(std::make_shared<FObjMesh>(FModulePath::MakeExeRelative("../../assets/shadowtest.obj").data(), *pDeviceResources)));

			//Pass setup
		SceneGraph.m_vActors.at(0)->GetRenderPasses() = { pStandardPass };

			//matrix transforms
		SceneGraph.m_vActors.at(0)->Translate(DirectX::XMVectorSet(0, .75, 0, 0));
		SceneGraph.m_vActors.at(0)->Scale(.25f, .25f, .25f);

		
		//1
		SceneGraph.m_vActors.push_back(std::make_unique<FActor>(std::make_shared<FObjMesh>(FModulePath::MakeExeRelative("../../assets/PlaneRot.obj").data(), *pDeviceResources)));
			//Pass setup
		SceneGraph.m_vActors.at(1)->GetRenderPasses() = { pShadowMapPass };
		SceneGraph.m_vActors.at(1)->Translate(DirectX::XMVectorSet(-0.5, .75, 0, 0));
		SceneGraph.m_vActors.at(1)->Scale(.75, .75, .75);
		
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

		*/

		CoUninitialize();

		return 0;

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
