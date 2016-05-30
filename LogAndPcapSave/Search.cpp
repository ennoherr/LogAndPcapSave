#include "stdafx.h"

#include <algorithm>
#include <string>
#include <iostream>

#include "Search.h"


Search::Search(void)
{
}

Search::~Search(void)
{
}

bool Search::isInString(std::string hay, std::string needle)
{
	bool res = false;

	if (hay.length() > 0 && needle.length() > 0)
	{
		transform(hay.begin(), hay.end(), hay.begin(), ::toupper);
		transform(needle.begin(), needle.end(), needle.begin(), ::toupper);

		size_t found = hay.find(needle);
		if (found != std::string::npos)
		{
			res = true;
		}
	}

	return res;
}

