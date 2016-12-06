#ifndef LOGCAPTURE_H
#define LOGCAPTURE_H

#include <string>

class LogCapture {
public:
    LogCapture(const std::string file = "");
    LogCapture(const LogCapture& orig);
    virtual ~LogCapture();
    
    void setLog(const std::string file);
    
private:
    std::string file_;
};

#endif /* LOGCAPTURE_H */

