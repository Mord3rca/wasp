#ifndef WASP_APDESYNC_HPP
#define WASP_APDESYNC_HPP

#include "mode_base.hpp"
#include "child.hpp"
#include "nexio.hpp" //Be sure to use monitor mode

#include <fstream>
#include <string>
#include <vector>

/*
 * Deauth APs with MDK3 (thats cruel)
 */

class apdesync : public mode_base
{
  const std::string MDK3_CMD = "mdk3 wlan0 d -b /tmp/mdk3_blacklist";
public:
  apdesync();
  ~apdesync();
  
  const std::string getName( void ) const;
  const std::string getOpCode( void ) const;
  
  void execOpCode( unsigned int, void* = nullptr );
  
  #ifdef WASP_DEBUG
  const std::string print( void ) const;
  #endif
  
private:
  const std::string _name = "AP Desync";
  
  child *_mdk3;
  
  std::vector< std::string >  _aps;
  std::fstream                _mdk3_blacklist;
  
  void _rewrite_file();
};

#endif //WASP_APDESYNC_HPP
