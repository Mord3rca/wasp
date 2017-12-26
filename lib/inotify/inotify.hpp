#ifndef INOTIFY_HPP
#define INOTIFY_HPP

//Cauz' it's multi-theaded ... On a single core. (who said RP3 ?)
#include <thread>
#include <mutex>
#include <atomic>

#include <chrono>

#include <string>

#include <functional>

#include <fstream>
#include <cstdio>

extern "C"
{
  #include <string.h> /* bzero, memset */
  #include <sys/inotify.h> /* Used to get file modification trigger */
  #include <fcntl.h>
  #include <unistd.h>
  //TODO: Will be used to add a timeout for Inotify
  //#include <sys/select.h> /* select(), struct timeval */
}

#define EVENT_SIZE (sizeof (struct inotify_event) )
#define EVENT_BUFF_SIZE ( 64 * (EVENT_SIZE + 16) )

class inotify_watcher
{
public:
  inotify_watcher(const std::string, int = IN_CREATE | IN_MODIFY);
  ~inotify_watcher();
  
  void setCallback( void(*)(int, const std::string) );
  void setCallback( std::function< void(int, const std::string) > );
  
  void start();
  void stop();
  
  bool running( void );
  
private:
  int fd, wd;
  std::function< void(int, const std::string) > _callback;
  
  std::thread       *_th;
  std::atomic<bool> _run;
  
  void worker();
};

#endif //INOTIFY_HPP
