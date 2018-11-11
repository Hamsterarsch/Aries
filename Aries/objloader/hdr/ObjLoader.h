#pragma once
#include <memory>
#include <string>

namespace ObjLoader
{
	struct FVector2
	{
		float X, Y;


	};

	struct FVector3
	{
		float X, Y, Z;
		

	};

	struct FVertex
	{
		FVector3 Position;
		FVector2 Uv;
		FVector3 Normal;


	};

	class FObjLoader
	{
	public:
		FObjLoader();
		FObjLoader(FObjLoader &&Other);

		~FObjLoader();

		FObjLoader &operator=(FObjLoader &&Rhs);


		void LoadObjFile(const char *pPath);
		const void *GetVertexData() const;
		const void *GetIndexData() const;
		size_t GetVertexDataMemsize() const;
		size_t GetIndexDataMemsize() const;
		size_t GetVertexDataSize() const;
		size_t GetIndexDataSize() const;

		void InvertWindingOrder();
		

	private:
		void LoadStringFromFile(const char *pPath, std::string &out_Buffer) const;
		

		struct Pimpl;
		std::unique_ptr<Pimpl> m_pImpl;


	};


}