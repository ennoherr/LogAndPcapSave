#pragma once

// stackoverflow.com/questions/3687759/monitoring-a-file-for-changes


#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

class Tail
{
public:
	Tail(std::string fname)
		: firstRun_(true) // on first run filesizes are equal
	{
		// init - get current size
		fname_ = fname;
		std::ifstream f(fname_);
		if (f.is_open())
		{
			f.seekg(0, f.end);
			fend_ = f.tellg();
		}
	};
	~Tail()
	{
		// nothing to do
	};

	// file has changed, e.g. detect with stat(...)
	void OnChange()
	{
		// re-open to get new size, cannot be done on the fly
		std::ifstream f(fname_.c_str());
		if (f.is_open())
		{
			f.seekg(0, f.end);
			std::streampos fend_new = f.tellg();

			// data added/removed or first time run
			if (fend_new > fend_  || fend_new < fend_ || firstRun_)
			{
				// remember changes and set pointer
				std::streamoff change = fend_new - fend_;

				// read whole file
				if (change < 0 || firstRun_)
				{
					change = fend_;
					f.seekg(0, f.beg);

					firstRun_ = false;
				}
				// only added lines
				else
				{
					f.seekg(fend_, f.beg);
				}

				// write changes to buffer
				data_.clear();
				data_.resize((unsigned)change + 1);
				f.read(&data_.front(), change);

#ifdef _DEBUG
				// show changes
				std::cout << "change " << change << " bytes" << std::endl;
				std::cout << "buffer: " << std::endl << &data_.front() << std::endl << "-----" << std::endl;
#endif
			}

			// safe new end
			fend_ = fend_new;
		}
	};

	std::vector<char> GetChanges(void)
	{
		return data_;
	}

	std::string GetChangesAsString(void)
	{
		std::stringstream ss;
		for(char c : data_)
		{
			ss << c;
		}
		return ss.str();
	}

private:
	bool firstRun_;
	std::string fname_;
	std::vector<char> data_;
	std::ios::streampos fend_;
	

};

