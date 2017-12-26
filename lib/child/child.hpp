#ifndef CHILD_HPP
#define CHILD_HPP

#ifdef DEBUG_CHILD
  #define CHD_DBG(x) std::cerr << "[CHD DEBUG] " << x << std::endl;
#else
  #define CHD_DBG(x)
#endif

#include <iostream>

#include <functional>
#include <exception>
#include <sstream>
#include <vector>
#include <array>

#include <ext/stdio_filebuf.h>

extern "C"
{
  #include <unistd.h>
  #include <signal.h>
  #include <sys/wait.h>
  #include <sys/stat.h>
  #include <string.h>
}

void sigchld_handler(int);

class child
{
public:
  child( char**, const std::size_t);
  ~child();
  
  const pid_t getPid( void );
  bool isRunning( void );
  void pkill( int = SIGINT );
  
  std::string getName( void ) const;
  
  std::ostream& stdin ( void ) const noexcept;
  std::istream& stdout( void ) const noexcept;
  std::istream& stderr( void ) const noexcept;
  
private:
  pid_t _pid;
  char** _args; std::size_t _args_size;
  
  int _status;
  
  std::array<__gnu_cxx::stdio_filebuf<char>*, 3>  _filebufs;
  std::istream  *_stdout;
  std::ostream  *_stdin;
  std::istream  *_stderr;
  
  void _create();
  void _cleanUp();
};

class childManager
{
public:
  childManager();
  ~childManager();
  
  child*  spawn( const std::string& );
  std::vector< child* >::size_type count( void ) const noexcept;
  
  child* operator[](unsigned int) const;
  
private:
  std::vector< child* > _childs;
  static int _sighandler(int);
};

extern childManager childs;

#endif //CHILD_HPP
