#include "objloader/hdr/ObjLoader.h"
#include <fstream>
#include <memory>
#include <vector>
#include "objloader/hdr/ObjLexer.h"


namespace ObjLoader
{
	struct FObjLoader::Pimpl
	{
		std::string m_ObjString;
		FObjLexer m_ObjLexer;

		std::vector<FVertex> m_vLoadedVertices;
		std::vector<unsigned int> m_vLoadedIndices;
		std::vector<FVector3> m_vLoadedPositions;
		std::vector<FVector3> m_vLoadedNormals;
		std::vector<FVector2> m_vLoadedUvs;
		std::map<std::string, size_t> m_mVertHashToTableIndex;

	};

	FObjLoader::FObjLoader() :
		m_pImpl{ std::make_unique<Pimpl>() }
	{}

	FObjLoader::FObjLoader(FObjLoader &&Other) = default;
	FObjLoader::~FObjLoader() = default;
	FObjLoader &FObjLoader::operator=(FObjLoader &&Rhs) = default;

	size_t FObjLoader::GetVertexDataMemsize() const
	{
		return m_pImpl->m_vLoadedVertices.size() * sizeof(decltype(m_pImpl->m_vLoadedVertices)::value_type);

	}

	size_t FObjLoader::GetIndexDataMemsize() const
	{
		return m_pImpl->m_vLoadedVertices.size() * sizeof(decltype(m_pImpl->m_vLoadedIndices)::value_type);

	}

	size_t FObjLoader::GetVertexDataSize() const
	{
		return m_pImpl->m_vLoadedVertices.size();

	}

	size_t FObjLoader::GetIndexDataSize() const
	{
		return m_pImpl->m_vLoadedIndices.size();

	}

	void FObjLoader::InvertWindingOrder()
	{
		for (size_t Index{}; Index < m_pImpl->m_vLoadedVertices.size(); Index += 3)
		{
			auto Vert1 = m_pImpl->m_vLoadedVertices.at(Index);
			m_pImpl->m_vLoadedVertices.at(Index) = m_pImpl->m_vLoadedVertices.at(Index + 2);
			m_pImpl->m_vLoadedVertices.at(Index + 2) = Vert1;


		}
		

	}

	void FObjLoader::LoadObjFile(const char *pPath)
	{
		LoadStringFromFile(pPath, m_pImpl->m_ObjString);
		size_t PositionLine{}, UvLine{}, NormalLine{};

		size_t StringIndicator{};

		EObjToken FoundSection{ m_pImpl->m_ObjLexer.IdentifySectionToken(m_pImpl->m_ObjString, StringIndicator) };		
		while (FoundSection != EObjToken::EoF)
		{
			if (FoundSection == EObjToken::Vertex)
			{
				m_pImpl->m_vLoadedPositions.emplace_back();				
				auto &InputVertex{ m_pImpl->m_vLoadedPositions.back() };

				InputVertex.X = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
				++StringIndicator;

				InputVertex.Y = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
				++StringIndicator;

				InputVertex.Z = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
				

			}
			else if (FoundSection == EObjToken::Normal)
			{
				m_pImpl->m_vLoadedNormals.emplace_back();
				auto &InputVertex{ m_pImpl->m_vLoadedNormals.back() };

				InputVertex.X = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
				++StringIndicator;

				InputVertex.Y = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
				++StringIndicator;

				InputVertex.Z = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
		

			}
			else if (FoundSection == EObjToken::Uv)
			{
				m_pImpl->m_vLoadedUvs.emplace_back();
				auto &InputVertex{ m_pImpl->m_vLoadedUvs.back() };

				InputVertex.X = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
				++StringIndicator;

				InputVertex.Y = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
				

			}
			else if (FoundSection == EObjToken::Poly)
			{
				for (short int VertexCount{}; VertexCount < 3; ++VertexCount)
				{
					auto VertexHash{ m_pImpl->m_ObjLexer.GetVertexHash(m_pImpl->m_ObjString, StringIndicator) };

					//Vertex for this hash already exists...
					if (!m_pImpl->m_mVertHashToTableIndex.count(VertexHash))
					{
						m_pImpl->m_mVertHashToTableIndex[VertexHash] = m_pImpl->m_vLoadedVertices.size(); //Add the index of the to be created vertex

						auto PosIndex = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
						auto UvIndex = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
						auto NormalIndex = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
						++StringIndicator;

						m_pImpl->m_vLoadedVertices.emplace_back();
						auto &CreatedVert{ m_pImpl->m_vLoadedVertices.back() };

						CreatedVert.Position = m_pImpl->m_vLoadedPositions.at(PosIndex - 1);
						if (UvIndex > 0)
						{
							CreatedVert.Uv = m_pImpl->m_vLoadedUvs.at(UvIndex - 1);
						}

						if (NormalIndex > 0)
						{
							CreatedVert.Normal = m_pImpl->m_vLoadedNormals.at(NormalIndex - 1);
						}

					}
					m_pImpl->m_vLoadedIndices.push_back(m_pImpl->m_mVertHashToTableIndex.at(VertexHash));
				
				}

				if (m_pImpl->m_ObjString.at(StringIndicator - 1) != '\n')//todo: remove redundancy
				{
					auto VertexHash{ m_pImpl->m_ObjLexer.GetVertexHash(m_pImpl->m_ObjString, StringIndicator) };

					//Vertex for this hash already exists...
					if (!m_pImpl->m_mVertHashToTableIndex.count(VertexHash))
					{
						m_pImpl->m_mVertHashToTableIndex[VertexHash] = m_pImpl->m_vLoadedVertices.size(); //Add the index of the to be created vertex

						auto PosIndex = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
						auto UvIndex = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
						auto NormalIndex = m_pImpl->m_ObjLexer.ReadDataSection(m_pImpl->m_ObjString, StringIndicator);
						//++StringIndicator; the indicator should now point to a newline character that we dont want to skip

						m_pImpl->m_vLoadedVertices.emplace_back();
						auto &CreatedVert{ m_pImpl->m_vLoadedVertices.back() };

						CreatedVert.Position = m_pImpl->m_vLoadedPositions.at(PosIndex - 1);
						if (UvIndex > 0)
						{
							CreatedVert.Uv = m_pImpl->m_vLoadedUvs.at(UvIndex - 1);
						}

						if (NormalIndex > 0)
						{
							CreatedVert.Normal = m_pImpl->m_vLoadedNormals.at(NormalIndex - 1);
						}

					}
					m_pImpl->m_vLoadedIndices.push_back(m_pImpl->m_vLoadedVertices.size() - 1 - 1);
					m_pImpl->m_vLoadedIndices.push_back(m_pImpl->m_mVertHashToTableIndex.at(VertexHash));
					m_pImpl->m_vLoadedIndices.push_back(m_pImpl->m_vLoadedVertices.size() - 1 - 3);


				}



			}

			FoundSection = m_pImpl->m_ObjLexer.SeekNextSection(m_pImpl->m_ObjString, StringIndicator);

		}
		m_pImpl->m_vLoadedNormals.resize(0);
		m_pImpl->m_vLoadedPositions.resize(0);
		m_pImpl->m_vLoadedUvs.resize(0);
		m_pImpl->m_mVertHashToTableIndex.clear();

	}
		 
	const void *FObjLoader::GetVertexData() const
	{
		return m_pImpl->m_vLoadedVertices.data();


	}

	const void *FObjLoader::GetIndexData() const
	{
		return m_pImpl->m_vLoadedIndices.data();


	}


	void FObjLoader::LoadStringFromFile(const char *pPath, std::string &out_Buffer) const
	{
		std::fstream File{ pPath, std::fstream::in | std::fstream::ate };
		if (File.is_open())
		{

			auto FileLength = File.tellg();
			File.seekg(0, std::fstream::beg);

			auto pBuffer = std::make_unique<char[]>(FileLength);
			File.read(pBuffer.get(), FileLength);

			out_Buffer = pBuffer.release();
		
		}


	}



}