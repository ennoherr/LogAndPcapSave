#include "stdafx.h"

#include <algorithm>
#include <string>
#include <iostream>

#include "Search.h"


using namespace std;



CSearch::CSearch(void)
{
}


CSearch::~CSearch(void)
{
}

bool CSearch::IsInString(wstring hay, wstring needle)
{
	bool bRet = false;

	if (hay.length() > 0 && needle.length() > 0)
	{
		transform(hay.begin(), hay.end(), hay.begin(), ::toupper);
		transform(needle.begin(), needle.end(), needle.begin(), ::toupper);

		size_t found = hay.find(needle);
		if (found != string::npos)
		{
			bRet = true;
		}
	}

	return bRet;
}