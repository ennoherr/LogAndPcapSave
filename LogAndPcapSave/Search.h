#pragma once

#include <string>

using namespace std;

class CSearch
{
public:
	CSearch(void);
	~CSearch(void);

	bool IsInString(wstring hay, wstring needle);
};

