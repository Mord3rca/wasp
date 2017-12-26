#ifndef LOGS_HPP
#define LOGS_HPP

#include <fstream>
#include <mutex>

class logFile
{
public:
  logFile();
  logFile(const std::string);
  ~logFile();
  
  enum logLevel
  {
    LOG_INFO,
    LOG_WARN,
    LOG_CRIT
  };
  
  void operator()(const std::string, enum logLevel = LOG_INFO );
  
private:
  std::fstream* _log;
  std::mutex    _mtx;
};

#endif //LOGS_HPP