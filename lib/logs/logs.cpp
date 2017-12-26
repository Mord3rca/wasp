#include "logs.hpp"

logFile::logFile() : _log(nullptr)
{
}

logFile::logFile(const std::string filename)
{
  _log = new std::fstream(filename, std::fstream::out);
}

logFile::~logFile()
{
  if( _log )
  {
    _log->close();
    delete _log;
  }
}

void logFile::operator ()(const std::string msg, enum logLevel lvl)
{
  _mtx.lock();
  if( _log )
  {
    switch(lvl)
    {
      case LOG_INFO:
        *_log << "[*] ";
        break;
      case LOG_WARN:
        *_log << "[~] ";
        break;
      case LOG_CRIT:
        *_log << "[-] ";
        break;
      default:
      ;
    }
    *_log << msg << std::endl;
  }
  _mtx.unlock();
}
