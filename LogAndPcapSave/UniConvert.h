#pragma once

#include <string>

#if UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

class UniConvert
{
public:
	UniConvert(void);
	~UniConvert(void);

	void WCharToChar(const WCHAR* Src, char* Dest, int Size);
	void CharToWChar(const char* Src, WCHAR* Dest, int Size);
	void TCharToChar(const TCHAR* Src, char* Dest, int Size);
	void TCharToWChar(const TCHAR* Src, wchar_t* Dest, int Size);
	void CharToTChar(const char* Src, TCHAR* Dest, int Size);

	bool AnsiToUnicode16(CHAR *in_Src, WCHAR *out_Dst, INT in_MaxLen);
	bool AnsiToUnicode16L(CHAR *in_Src, INT in_SrcLen, WCHAR *out_Dst, INT in_MaxLen);

	std::wstring s2ws(const std::string& str);
	std::wstring ts2ws(const tstring& tstr);

	std::string ws2s(const std::wstring& wstr);
	std::string ts2s(const tstring& tstr);

	tstring ws2ts(const std::wstring& wstr);
	tstring s2ts(const std::string& str);

};

