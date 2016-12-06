#include <string>

#include "Tail.h"

#include "LogCapture.h"


LogCapture::LogCapture(const std::string file) 
{
    if (file.length()) setLog(file);
}

LogCapture::LogCapture(const LogCapture& orig) 
{
}

LogCapture::~LogCapture() 
{
}

void LogCapture::setLog(const std::string file)
{
    file_ = file;
}

