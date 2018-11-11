#include "objloader/hdr/ObjLexer.h"
#include <cctype>

namespace ObjLoader
{
	const FObjLexer::TokenMap FObjLexer::s_mSectionTokens
	{ 
		std::make_pair(EObjToken::Comment,		"#"	),
		std::make_pair(EObjToken::Vertex,		"v"	),
		std::make_pair(EObjToken::Normal,		"vn"),
		std::make_pair(EObjToken::Uv,			"vt"),
		std::make_pair(EObjToken::Poly,			"f"	),


	};

	const std::map<std::string, EObjToken>  FObjLexer::FObjLexer::s_mSectionTokensRev
	{ 
		std::make_pair(	"#"	, EObjToken::Comment ),
		std::make_pair(	"v"	, EObjToken::Vertex	 ),
		std::make_pair(	"vn", EObjToken::Normal	 ),
		std::make_pair(	"vt", EObjToken::Uv		 ),
		std::make_pair(	"f"	, EObjToken::Poly	 ),


	};

	const FObjLexer::TokenMap FObjLexer::s_mSeperatorTokens
	{
		std::make_pair(EObjToken::Slash,		"/"	),
		std::make_pair(EObjToken::Space, 		" "	),
		std::make_pair(EObjToken::Newline,		"\n")


	};

	EObjToken FObjLexer::SkipToNextToken(const std::string &ObjString, size_t &out_Indicator, const TokenMap &SearchMap) const
	{		
		auto LineEnd = ObjString.find_first_of("\n", out_Indicator);
		if (LineEnd == std::string::npos)
		{
			return EObjToken::EoF;

		}
		
		auto FoundObjToken = EObjToken::None;
		auto NearestTokenPos = std::string::npos;
		for (auto &&Pair : SearchMap)
		{		
			auto TokenPos = ObjString.find(Pair.second, out_Indicator, LineEnd);
			if ( TokenPos != std::string::npos )
			{
				//Update if this token is closer to the range start
				//than all previous tokens
				if (TokenPos < NearestTokenPos)
				{
					FoundObjToken = Pair.first;
					NearestTokenPos = TokenPos;
				
				}

				//If token is the first character of the range 
				//no other token can be before
				if (TokenPos == out_Indicator)
				{
					break;
				
				}

			}

		}

		out_Indicator = NearestTokenPos+1;
		return FoundObjToken;

	}

	void FObjLexer::SkipWhitespaces(const std::string &ObjString, size_t &out_Indicator) const
	{
		out_Indicator = ObjString.find_first_not_of(" \n\t", out_Indicator);


	}

	float FObjLexer::ReadDataSection(const std::string &ObjString, size_t &out_Indicator) const
	{
		auto TokenEndPos{ ObjString.find_first_of(" \n/", out_Indicator) };
		auto Token{ ObjString.substr(out_Indicator, TokenEndPos - out_Indicator) };

		if (TokenEndPos == out_Indicator)
		{
			TokenEndPos += 2;//check: used for poly double slashes
		}
		out_Indicator = TokenEndPos;

		return strtof(Token.data(), nullptr);

		
	}

	std::string FObjLexer::GetVertexHash(const std::string &ObjString, size_t Indicator) const
	{
		auto TokenEndPos{ ObjString.find_first_of(" \n", Indicator) };
		return ObjString.substr(Indicator, TokenEndPos - Indicator);

	}

	void FObjLexer::FindNextDataSection(const std::string &ObjString, size_t &out_Indicator) const
	{
		auto LineEndPos{ ObjString.find('\n', out_Indicator) };
		auto NextNonWhitespace{ ObjString.find_first_not_of(" \n", out_Indicator) };

		//NextNonWhitespace < LineEndPos


	}

	EObjToken FObjLexer::SeekNextSection(const std::string &ObjString, size_t &out_Indicator) const
	{
		SkipToNextSection(ObjString, out_Indicator);
		return IdentifySectionToken(ObjString, out_Indicator);


	}

	//skips to pos after next newline
	void FObjLexer::SkipToNextSection(const std::string &ObjString, size_t &out_Indicator) const
	{
		out_Indicator = ObjString.find_first_of('\n', out_Indicator) + 1;
		

	}

	EObjToken FObjLexer::SkipToNextSeperator(const std::string &ObjString, size_t &out_Indicator) const
	{
		return SkipToNextToken(ObjString, out_Indicator, s_mSeperatorTokens);
		

	}

	//prec: indic at token start, indic set to the position after the space after the token
	EObjToken FObjLexer::IdentifySectionToken(const std::string &ObjString, size_t &out_Indicator) const
	{
		auto TokenEndPos{ ObjString.find_first_of(" \n", out_Indicator) };
		auto Token{ ObjString.substr(out_Indicator, TokenEndPos - out_Indicator) };

		if (s_mSectionTokensRev.count(Token))
		{
			out_Indicator = TokenEndPos + 1;
			return s_mSectionTokensRev.at(Token);

		}
		return TokenEndPos != std::string::npos ? EObjToken::None : EObjToken::EoF;


	}

	void FObjLexer::SkipToNextNewline(const std::string &ObjString, size_t &out_Indicator) const
	{
		out_Indicator = ObjString.find_first_of('\n', out_Indicator);


	}


}