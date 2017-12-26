#include "server.hpp"

//Main should Init this according to Args.
http::server *serv = nullptr;

std::array< const std::string, 3 > mime_json = { "application/json", "text/json", "text/javascript" };

void server_callback( const http::request &req, http::answer &ans )
{
  if( req.path == "/" )
  {
    ans.content << "Hey ! Glad to serv you :)" << std::endl
                << "FYI the mode currently running is " << current_mode->getName() << std::endl;
  }
  #ifdef WASP_DEBUG
  else if(req.path == "/debug")
  {
    serv_debug( ans.content );
  }
  #endif
  else if( is_mime_type_json( req.getHeader("Content-Type") ) )
  {
    std::cout << req.content.str() << std::endl;
    ans.content << "Understood." << std::endl;
  } 
  else
  {
    ans.status  = 404;
    ans.content << "Request not understood ..." << std::endl;
  }
}

bool is_mime_type_json( const std::string &mime )
{
  for(auto i : mime_json)
    if( i == mime )
      return true;
  
  return false;
}

#ifdef WASP_DEBUG
void serv_debug( std::ostream& info_stream )
{
  info_stream << "---------- DEBUG ----------" << std::endl
              << "-- APS Count: " << list->apList().size() << std::endl;
  
  for( const target_list::ap* i : list->apList() )
  {
    info_stream << "     " << i->BSSID << " - " << i->ESSID << std::endl;
  }
  
  info_stream << std::endl
              << "-- Stations Count: " << list->stationList().size() << std::endl;
  
  for( const target_list::station *i : list->stationList() )
  {
    info_stream << "     "  << i->MAC << " - "
                            << ((i->associated_ap != nullptr) ? i->associated_ap->ESSID +" (" + i->associated_ap->BSSID + ")" : "Not Associated")
                            << std::endl;
  }
  
  if(nx)
    info_stream << "NEXIO Started, monitor " << (nx->monitor() == 2 ? "enabled" : "disabled") <<std::endl;
  else
    info_stream << "NEXIO desactivated." << std::endl;
  
  if( current_mode )
    info_stream << std::endl << std::endl << current_mode->print();
}
#endif
