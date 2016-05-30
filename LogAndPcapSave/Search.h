#pragma once

#include <string>

class Search
{
public:
	Search(void);
	~Search(void);

	bool isInString(std::string hay, std::string needle);
};

