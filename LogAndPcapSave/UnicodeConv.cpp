#include "stdafx.h"

#include <string>
#include <codecvt>

#include "UnicodeConv.h"

using namespace std;

#if UNICODE
#define tstring wstring
#else
#define tstring string
#endif

CUnicodeConv::CUnicodeConv(void)
{
}


CUnicodeConv::~CUnicodeConv(void)
{
}

void CUnicodeConv::WCharToChar(const WCHAR* Src, char* Dest, int Size)
{
	WideCharToMultiByte(CP_ACP, 0, Src, wcslen(Src)+1, Dest , Size, NULL, NULL); 
}

void CUnicodeConv::CharToWChar(const char* Src, WCHAR* Dest, int Size)
{
	MultiByteToWideChar(CP_ACP, 0, Src, -1, Dest, Size);
}

void CUnicodeConv::TCharToChar(const TCHAR* Src, char* Dest, int Size) 
{
#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, Src, wcslen(Src)+1, Dest , Size, NULL, NULL); 
#else
	wcscpy_s(Dest, Size, Src);
#endif
} 

void CUnicodeConv::TCharToWChar(const TCHAR* Src, wchar_t* Dest, int Size)
{
#ifdef UNICODE
	wcscpy_s(Dest, Size, Src);
#else
	MultiByteToWideChar(CP_ACP, 0, Src, -1, Dest, Size);
#endif
}

void CUnicodeConv::CharToTChar(const char* Src, TCHAR* Dest, int Size)
{
#ifdef UNICODE
	MultiByteToWideChar(CP_ACP, 0, Src, -1, Dest, Size);
#else
	strcpy_s(Dest, Size, Src);
#endif	
}


/***************************/
/* ansi-unicode conversion */
/***************************/

bool CUnicodeConv::AnsiToUnicode16(CHAR *in_Src, WCHAR *out_Dst, INT in_MaxLen)
{
	/* locals */
	INT lv_Len;

	// do NOT decrease maxlen for the eos
	if (in_MaxLen <= 0)
		return false;

	// let windows find out the meaning of ansi
	// - the SrcLen=-1 triggers MBTWC to add a eos to Dst and fails if MaxLen is too small.
	// - if SrcLen is specified then no eos is added
	// - if (SrcLen+1) is specified then the eos IS added
	lv_Len = MultiByteToWideChar(CP_ACP, 0, in_Src, -1, out_Dst, in_MaxLen);

	// validate
	if (lv_Len < 0)
		lv_Len = 0;

	// ensure eos, watch out for a full buffersize
	// - if the buffer is full without an eos then clear the output like MBTWC does
	//   in case of too small outputbuffer
	// - unfortunately there is no way to let MBTWC return shortened strings,
	//   if the outputbuffer is too small then it fails completely
	if (lv_Len < in_MaxLen)
		out_Dst[lv_Len] = 0;
	else if (out_Dst[in_MaxLen-1])
		out_Dst[0] = 0;

	// done
	return true;
}


bool CUnicodeConv::AnsiToUnicode16L(CHAR *in_Src, INT in_SrcLen, WCHAR *out_Dst, INT in_MaxLen)
{
	/* locals */
	INT lv_Len;


	// do NOT decrease maxlen for the eos
	if (in_MaxLen <= 0)
		return false;

	// let windows find out the meaning of ansi
	// - the SrcLen=-1 triggers MBTWC to add a eos to Dst and fails if MaxLen is too small.
	// - if SrcLen is specified then no eos is added
	// - if (SrcLen+1) is specified then the eos IS added
	lv_Len = MultiByteToWideChar(CP_ACP, 0, in_Src, in_SrcLen, out_Dst, in_MaxLen);

	// validate
	if (lv_Len < 0)
		lv_Len = 0;

	// ensure eos, watch out for a full buffersize
	// - if the buffer is full without an eos then clear the output like MBTWC does
	//   in case of too small outputbuffer
	// - unfortunately there is no way to let MBTWC return shortened strings,
	//   if the outputbuffer is too small then it fails completely
	if (lv_Len < in_MaxLen)
		out_Dst[lv_Len] = 0;
	else if (out_Dst[in_MaxLen-1])
		out_Dst[0] = 0;

	// done
	return true;
}

// http://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
// 2015-06-08
wstring CUnicodeConv::s2ws(const string& str)
{
    typedef codecvt_utf8<wchar_t> convert_typeX;
    wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

string CUnicodeConv::ws2s(const std::wstring& wstr)
{
    typedef codecvt_utf8<wchar_t> convert_typeX;
    wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

wstring CUnicodeConv::ts2ws(const tstring& tstr)
{
#if UNICODE
	return tstr;
#else
	return s2ws(tstr);
#endif
}

string CUnicodeConv::ts2s(const tstring& tstr)
{
#if UNICODE
	return ws2s(tstr);
#else
	return tstr;
#endif
}

tstring CUnicodeConv::ws2ts(const wstring& wstr)
{
#if UNICODE
    return wstr;
#else
	return ws2s(wstr);
#endif

}

tstring CUnicodeConv::s2ts(const string& str)
{
#if UNICODE
	return s2ws(str);
#else
	return str;
#endif
}
