#pragma once
#include <map>

namespace ObjLoader
{
	enum class EObjToken : unsigned short
	{
		None,
		Comment,
		Vertex,
		Normal,
		Uv,
		Poly,
		Object,
		Space,
		Slash,
		Newline,
		EoF


	};

	class FObjLexer
	{
	public:
		using TokenMap = std::map<EObjToken, const char*>;
		EObjToken SeekNextSection(const std::string &ObjString, size_t &out_Indicator) const;
		EObjToken IdentifySectionToken(const std::string &ObjString, size_t &out_Indicator) const;
		std::string GetVertexHash(const std::string &ObjString, size_t Indicator) const;

		void SkipWhitespaces(const std::string &ObjString, size_t &out_Indicator) const;
		float ReadDataSection(const std::string &ObjString, size_t &out_Indicator) const;
		void FindNextDataSection(const std::string &ObjString, size_t &out_Indicator) const;

		void SkipToNextNewline(const std::string &ObjString, size_t &out_Indicator) const;
		EObjToken SkipToNextSeperator(const std::string &ObjString, size_t &out_Indicator) const;

	private:
		void SkipToNextSection(const std::string &ObjString, size_t &out_Indicator) const;


		EObjToken SkipToNextToken(const std::string &ObjString, size_t &out_Indicator, const TokenMap &SearchMap) const;

		static const TokenMap s_mSectionTokens;
		static const TokenMap s_mSeperatorTokens;
		static const std::map<std::string, EObjToken> s_mSectionTokensRev;

		size_t m_Indicator;



	};


}