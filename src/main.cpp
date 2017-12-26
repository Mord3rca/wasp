#include "main.hpp"

//For futur use.
#ifdef WASP_GW_ONLY
  nexioctl *nx = nullptr;
#else
  nexioctl *nx = new nexioctl;
#endif
target_list *list         = new target_list;

mode_base   *current_mode = nullptr;

void sig_handler( int )
{
  if( serv != nullptr )
    serv->stop();
    
  if( current_mode )
  {
    delete current_mode;
  }
  
  if( nx )
  {
    nx->monitor(0);
    delete nx;
  }
  
  delete list;
  
  std::exit(0);
}

int main(int argc, char *argv[])
{
  /*
   * parse_args(argc, argv);
   * 
   * if(daemon)
   *  daemonize();
   */
  signal(SIGINT, sig_handler);
  
  if(nx)
    nx->monitor(2);
  
  std::cout << "Starting Server" << std::endl;
  serv = new http::server(8080);
    serv->setCallback( server_callback );
    serv->start();
  
  std::cout << "Creating Sniffer Mode" << std::endl;
  if( current_mode == nullptr )
    current_mode = new sniffer;
  
  while(true)
  {
    sleep(1000);
  }
  
  return 0;
}
