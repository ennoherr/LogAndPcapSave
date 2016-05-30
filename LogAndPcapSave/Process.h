#pragma once

#include <string>

class processes
{
public:
	processes(void);
	~processes(void);

	bool isProcessRunning(const std::string processName);
};

