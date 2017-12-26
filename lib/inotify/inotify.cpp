#include "inotify.hpp"

inotify_watcher::inotify_watcher( const std::string target, int options) : _th(nullptr), _run(true)
{
  fd = inotify_init();
  
  if( fd < 0 )
    throw std::runtime_error("[INOTIFY] inotify_init() error.");
  
  wd = inotify_add_watch(fd, target.c_str(), options);
}

inotify_watcher::~inotify_watcher()
{
  stop();
  
  inotify_rm_watch(fd, wd);
  close(fd);
  
  if(_th)
    delete _th;
}

void inotify_watcher::setCallback( void(*func)(int, const std::string ) )
{
  _callback = func;
}

void inotify_watcher::setCallback( std::function< void(int, const std::string) > func )
{
  _callback = func;
}

void inotify_watcher::worker()
{
  std::string filename; char buffer[EVENT_BUFF_SIZE];
  
  while(_run)
  {
    long length = read(fd, &buffer, EVENT_BUFF_SIZE);
    if(length < 0)
      throw std::runtime_error( "[INOTIFY] worker - read() error." );
    
    long i = 0;
    while( i < length )
    {
      struct inotify_event *event = (struct inotify_event*)(&buffer[i]);
      if( event->len )
      {
        _callback( event->mask, event->name );
      }
      i += EVENT_SIZE + event->len;
    }
  }
}

void inotify_watcher::start()
{
  if( _th )
    delete _th;
    
  _run.store(true);
  
  _th = new std::thread( &inotify_watcher::worker, this );
}

void inotify_watcher::stop()
{
  _run.store(false);
  
  //Waiting for epoll stuff, for the moment: bad cleaning
  std::ofstream stop_inotify("/tmp/stop", std::fstream::out);
    stop_inotify << "STOP IT !";
    stop_inotify.close();
    
  remove("/tmp/stop");
  
  if(_th && _th->joinable())
    _th->join();
}

bool inotify_watcher::running( void )
{
  return _run;
}
