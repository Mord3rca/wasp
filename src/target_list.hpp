#ifndef WASP_TARGET_LIST_HPP
#define WASP_TARGET_LIST_HPP

#include <vector>
#include <string>
#include <sstream>

#include <mutex>

#include "utilities.hpp"
#include "csv.hpp"

class target_list
{
  
public:
  target_list();
  ~target_list();
  
  struct ap
  {
    ap();
    ap( ap& ); //Copy
    ap( CSVRow& ); //Creation via CSVRow
    
    std::string BSSID, first_seen, last_seen;
    int channel, speed;
    std::string privacy, cipher, authentification;
    int power, nbr_beacons, nbr_IV;
    std::string lan_ip;
    int id_length;
    std::string ESSID, key;
    
    std::string serialize() const;
  };
  
  struct station
  {
    station();
    station( CSVRow& );
    station( station& );
    
    std::string MAC, first_seen, last_seen;
    int power, nbr_packets;
    std::string BSSID, probed_BSSID;
    
    struct ap* associated_ap;
    
    std::string serialize() const;
  };
  
  const std::vector<ap*>&       apList() const;
  const std::vector<station*>&  stationList() const;
  
  void apList     ( const std::vector<ap*>& );
  void stationList( const std::vector<station*>& );
  
  enum serialize_request //Unused for the moment.
  {
    APS = 1,
    STATION = 2,
    ALL = 3
  };
  
  const std::string serialize( const serialize_request = ALL );
  
private:
  std::vector<ap*>      *_aps;
  std::vector<station*> *_stations;
  
  std::mutex            mtx;
};

extern target_list* list;

#endif //WASP_TARGET_LIST_HPP
