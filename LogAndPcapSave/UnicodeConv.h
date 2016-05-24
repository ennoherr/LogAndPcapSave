#pragma once

#include <string>

#if UNICODE
#define tstring wstring
#else
#define tstring string
#endif

using namespace std;

class CUnicodeConv
{
public:
	CUnicodeConv(void);
	~CUnicodeConv(void);

	void WCharToChar(const WCHAR* Src, char* Dest, int Size);
	void CharToWChar(const char* Src, WCHAR* Dest, int Size);
	void TCharToChar(const TCHAR* Src, char* Dest, int Size);
	void TCharToWChar(const TCHAR* Src, wchar_t* Dest, int Size);
	void CharToTChar(const char* Src, TCHAR* Dest, int Size);

	bool AnsiToUnicode16(CHAR *in_Src, WCHAR *out_Dst, INT in_MaxLen);
	bool AnsiToUnicode16L(CHAR *in_Src, INT in_SrcLen, WCHAR *out_Dst, INT in_MaxLen);

	wstring s2ws(const std::string& str);
	wstring ts2ws(const tstring& tstr);

	string ws2s(const std::wstring& wstr);
	string ts2s(const tstring& tstr);

	tstring ws2ts(const wstring& wstr);
	tstring s2ts(const string& str);

};

