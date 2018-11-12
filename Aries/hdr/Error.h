#pragma once
#include <wrl/client.h>

struct FError
{
	FError(HRESULT ReturnValue, const char *ErrorMessage, const char *CodeFilePath, int ErrorLineNumber) :
		m_ReturnValue{ ReturnValue },
		m_ErrorMessage{ ErrorMessage },
		m_CodeFilePath{ CodeFilePath },
		m_ErrorLineNumber{ ErrorLineNumber }
	{
	};

	FError(HRESULT ReturnValue, const char *ErrorMessage) :
		FError(ReturnValue, ErrorMessage, "", 0)
	{};


	HRESULT m_ReturnValue;
	const char *m_ErrorMessage;
	const char *m_CodeFilePath;
	int m_ErrorLineNumber;


};
