#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <ostream>
#include <sstream>
#include <cstdlib>
#include <cstring>

#include <vector>
#include <map>

#include <thread>
#include <atomic>

#include <string>

extern "C"
{
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
}

#include <ext/stdio_filebuf.h>

namespace http
{
  extern const std::string server_name;
  
  struct answer
  {
    answer();
    
    void addHeader(const std::string, const std::string);
    void send(std::ostream&);
    
    unsigned int status;
    std::map<std::string, std::string> headers;
    std::ostringstream content;
  };
  
  struct request
  {
    std::string path;
    enum methods
    {
      HTTP_GET  = 0,
      HTTP_POST = 1
    } method;
    
    const std::string getHeader( const std::string ) const;
    std::map< std::string, std::string > headers;
    std::istringstream content;
  };
  
  class server
  {
  public:
    server(int = 80);
    ~server();
  
    void start();
    void stop();
    
    void setCallback( void(*)(const struct request&, answer&) );
  
  private:
    int _sd, _port;
    
    std::atomic<bool> _run;
    std::thread *_th;
    
    void _request_handler();
    void _parse_request(struct request&, std::istream&);
    
    std::function< void(const struct request&, answer&) > _callback;
  };
}

#endif //HTTP_SERVER_HPP