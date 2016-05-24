#pragma once

#include <string>

using namespace std;

class CProcess
{
public:
	CProcess(void);
	~CProcess(void);

	bool IsProcessRunning(const wstring processName);
};

