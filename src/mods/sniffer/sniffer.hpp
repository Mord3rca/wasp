#ifndef WASP_SNIFF_HPP
#define WASP_SNIFF_HPP

#include <json/json.h>

#include "mode_base.hpp"
#include "nexio.hpp"

#include "child.hpp"
#include "inotify.hpp"
#include "csv.hpp"

#include "target_list.hpp"
#include "apdesync.hpp"

/* name: Sniffer
 * Type: Dyn Module (TODO: When plugin managment is done.)
 * 
 */

class sniffer : public mode_base
{
  const std::string AIRODUMP_CMD = "airodump-ng --berlin 30 --write /tmp/result --output-format csv wlan0";
  const time_t update_delay = 2;
  
public:
  sniffer();
  ~sniffer();
  
  const std::string getName( void ) const;
  const std::string getOpCode( void ) const;
  void execOpCode(unsigned int, void*);
  
  #ifdef WASP_DEBUG
  const std::string print( void ) const;
  #endif
  
  void stop( void );
  
  const std::string filename( void ) const;
  void filename( const std::string );
  
  void update( void );

private:
  const std::string _name = "Sniffer";
  
  child*            _airodump;
  std::string       _filename;
  inotify_watcher*  _inotify;
  
  time_t _lastsync;
  
  void _link_devices( std::vector<target_list::ap*>&,
                      std::vector<target_list::station*>& ) const;
};

void inotify_sniff_callback(int, const std::string);

#endif //WASP_SNIFF_HPP
