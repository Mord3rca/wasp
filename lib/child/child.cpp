#include "child.hpp"

void sigchld_handler( int e )
{
  pid_t pid; int status;
  
  //Kill all zombie child.
  while((pid = waitpid(-1, &status, WNOHANG)) > 0);
  CHD_DBG("A process died died.");
}

childManager::childManager()
{
  struct sigaction sig;
  sigemptyset( &sig.sa_mask );
  sig.sa_flags = 0;
  sig.sa_handler = sigchld_handler;
  
  sigaction(SIGCHLD, &sig, nullptr);
}

childManager::~childManager()
{
  for(auto i : _childs)
    i->pkill();
}

std::vector< child* >::size_type childManager::count( void ) const noexcept
{
  return _childs.size();
}

child* childManager::operator [](unsigned int index) const
{
  return _childs[index];
}

child* childManager::spawn( const std::string& e)
{
  CHD_DBG("Starting arguments list creation...");
  
  std::istringstream args(e); std::vector< std::string > args_list; std::string tmp;
  while( args >> tmp )
    args_list.push_back(tmp);
  
  char** c_args = new char*[args_list.size() +1];
  for( std::vector<std::string>::size_type i = 0; i < args_list.size(); i++ )
  {
    char* c_tmp = new char[ args_list[i].length() ];
    strcpy( c_tmp, args_list[i].c_str() );
    c_args[i] = c_tmp;
  }
  
  c_args[ args_list.size() ] = nullptr;
  
  child* element = new child(c_args, args_list.size() );
  _childs.push_back( element );
  
  CHD_DBG("End of arguments list creation...");
  
  return element;
}

child::child( char** args, const std::size_t size) :  _pid(0), _args(args), _args_size( size ),
                                                      _stdout(nullptr), _stdin(nullptr), _stderr(nullptr)
{
  _create();
}

child::~child()
{
  _cleanUp();
}

const pid_t child::getPid( void )
{
  return _pid;
}

bool child::isRunning( void )
{
  if(!_pid)
    return false;
  
  return waitpid(_pid, nullptr, WNOHANG) == 0;
}

void child::pkill( int sig )
{
  //if( isRunning() ) //Something is wrong with this ... Better to send the signal no matter what.
    kill(_pid, sig);
}

std::string child::getName( void ) const
{
  return std::string(_args[0]);
}

std::istream& child::stdout( void ) const noexcept
{
  return *_stdout;
}

std::ostream& child::stdin( void ) const noexcept
{
  return *_stdin;
}

std::istream& child::stderr( void ) const noexcept
{
  return *_stderr;
}

void child::_create()
{
  CHD_DBG("Creating child");
  int child_stdout[2], child_stderr[2], child_stdin[2];
  
  if( pipe(child_stdout) < 0 )
    throw std::runtime_error("Pipe error while creating " + std::string(_args[0]) + " child");
    
  if( pipe(child_stderr) < 0 )
  {
    close(child_stdout[0]);
    close(child_stdout[1]);
    throw std::runtime_error("Pipe error while creating " + std::string(_args[0]) + " child");
  }
  
  if( pipe(child_stdin) < 0 )
  {
    close(child_stdout[0]);
    close(child_stdout[1]);
    close(child_stderr[0]);
    close(child_stderr[1]);
    throw std::runtime_error("Pipe error while creating " + std::string(_args[0]) + " child");
  }
  
  CHD_DBG("Forking...");
  _pid = fork();
  if( _pid == 0 ) //Child
  {
    //TODO: Redirect to Parent Flag
    if( dup2( child_stdout[1], STDOUT_FILENO ) == -1)
      throw std::runtime_error("dup2: Couldn't duplicate STDOUT.");
    
    if( dup2( child_stdin [0], STDIN_FILENO  ) == -1)
      throw std::runtime_error("dup2: Couldn't duplicate STDIN.");
    
    if( dup2( child_stderr[1], STDERR_FILENO ) == -1)
      throw std::runtime_error("dup2: Couldn't duplicate STDERR.");
    
    umask(0);
    setpgrp();
    
    int nResult = execvp( _args[0], _args); exit(nResult);
    
    throw std::runtime_error("execvp: critical (" + std::to_string(nResult) + ")");;
  }
  else if( _pid > 0) //parent
  {
    CHD_DBG("Creating C++ style pipes...");
    close( child_stdin [0] ); //Close READ STDIN
    close( child_stdout[1] ); //Close WRITE STDOUT
    close( child_stderr[1] ); //Close WRITE STDERR
    
    _filebufs[0] = new __gnu_cxx::stdio_filebuf<char>(child_stdout[0], std::ios::in);
    _filebufs[1] = new __gnu_cxx::stdio_filebuf<char>(child_stdin[1], std::ios::out);
    _filebufs[2] = new __gnu_cxx::stdio_filebuf<char>(child_stderr[0], std::ios::in);
    
    _stdout = new std::istream( _filebufs[0] );
    _stdin  = new std::ostream( _filebufs[1] );
    _stderr = new std::istream( _filebufs[2] );
  }
  else //Deepshit
  {
    close(child_stdout[0]);
    close(child_stdout[1]);
    close(child_stderr[0]);
    close(child_stderr[1]);
    close(child_stdin [0]);
    close(child_stdin [1]);
    
    throw std::runtime_error("Child " + std::string(_args[0]) + ": Fork() error" );
  }
}

void child::_cleanUp()
{
  CHD_DBG("Cleaning up...");
  pkill();
  
  for(auto i : _filebufs)
  {
    if(i)
    {
      i->close();
      delete i;
    }
  }
  
  delete _stdout;
  delete _stdin;
  delete _stderr;
  
  for( std::size_t i = 0; i < _args_size; i++ )
    delete _args[i];
  
  delete _args;
}

childManager childs;
