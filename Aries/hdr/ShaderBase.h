#pragma once

#include <D3DCompiler.h>
#include <d3dcommon.h>
#include <D3D11.h>
#include <vector>
#include <memory>
#include "ModulePath.h"
#include "Error.h"
#include "IContextBinding.h"
//#include <bitset>


template<class ShdrType>
class FShaderBase : public IContextBinding
{
public:
	using pfShaderFunct = HRESULT(_stdcall ID3D11Device::*)(const void *, SIZE_T, ID3D11ClassLinkage *, ShdrType **);
	//file name, entry point, shmodel target
	//defines, includes, compile constants, class linkage
	//src name, compile effect constants

	FShaderBase
	(
		ID3D11Device *pDevice,
		LPCSTR EntryPoint,
		LPCWSTR Filepath,
		LPCSTR TargetModel,
		pfShaderFunct CreationFunct,
		const D3D_SHADER_MACRO *pDefines,
		ID3DInclude *pIncludes,
		UINT CompileConstants,
		ID3D11ClassLinkage *pClassLinkage,
		UINT CompileEffectConstants
	)
	{	
		auto Hr = D3DCompileFromFile
		(
			Filepath,
			pDefines,
			pIncludes,
			EntryPoint,
			TargetModel,
			CompileConstants,
			CompileEffectConstants,
			&m_pBytecode,
			&m_pErrorBlob
			
		);
		

		if (FAILED(Hr))
		{
			//todo: if does not work if the error code is ERROR_PATH_NOT_FOUND
			if (Hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
			{
				throw(FError{ Hr, "Could not open shader file.", __FILE__, __LINE__ });

			}

			std::string Str(reinterpret_cast<char *>(m_pErrorBlob->GetBufferPointer()));


			throw(FError{ Hr, "Could not compile shader from file.", __FILE__, __LINE__ });

		}

		Hr = (pDevice->*CreationFunct)(m_pBytecode->GetBufferPointer(), m_pBytecode->GetBufferSize(), pClassLinkage, &m_pShader);
		if (FAILED(Hr))
		{
			throw(FError{ Hr, "Could not create shader.", __FILE__, __LINE__ });

		}


	}

	FShaderBase(ID3D11Device *pDevice, LPCSTR EntryPoint, LPCWSTR Filepath, LPCSTR TargetModel, pfShaderFunct CreationFunct, const D3D_SHADER_MACRO *pDefines, ID3DInclude *pIncludes, UINT CompileConstants) :
		FShaderBase(pDevice, EntryPoint, Filepath, TargetModel, CreationFunct, pDefines, pIncludes, CompileConstants, nullptr, 0)
	{};
	FShaderBase(ID3D11Device *pDevice, LPCSTR EntryPoint, LPCWSTR Filepath, LPCSTR TargetModel, pfShaderFunct CreationFunct) :
		FShaderBase(pDevice, EntryPoint, Filepath, TargetModel, CreationFunct, nullptr, nullptr, 0, nullptr, 0)
	{};

	ShdrType *GetShader() const { return m_pShader; }
	ID3DBlob *GetBytecode() const { return m_pBytecode; }


private:	
	ShdrType *m_pShader;
	ID3DBlob *m_pErrorBlob, *m_pBytecode;
	//1 file errors; 2 compile errors, 3 create errors
	//std::bitset<3> m_ErrorField;


};

/*
class ReflectionVertex : public FShaderBase<ID3D11VertexShader>
{
public:
	virtual void Bind(ID3D11DeviceContext &Context) override
	{
		Context.VSSetShader(GetShader(), nullptr, 0);

	}

private:

};
*/