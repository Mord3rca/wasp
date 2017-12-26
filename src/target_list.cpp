#include "target_list.hpp"

target_list::ap::ap() {}

target_list::ap::ap( ap& _ap ) :  BSSID( _ap.BSSID ), first_seen( _ap.first_seen ), last_seen(_ap.last_seen),
                                  channel( _ap.channel ), speed( _ap.speed ), privacy( _ap.privacy ),
                                  cipher( _ap.cipher ), authentification( _ap.authentification ),
                                  power( _ap.power ), nbr_beacons( _ap.nbr_beacons ),
                                  nbr_IV( _ap.nbr_IV ), lan_ip( _ap.lan_ip ), id_length( _ap.id_length ),
                                  ESSID( _ap.ESSID ), key( _ap.key )
{
  //I wanted to use a memcpy here but ... I'm using some std::string soo ... bad idea :(
  //TODO: Pass all that shit in C style variable. (int, long, char* & stuff.)
}

target_list::ap::ap( CSVRow& row )
{
  if( row.size() >= 15 )
  {
    BSSID = strip( row[0] );
    first_seen = strip( row[1] ); last_seen = strip( row[2] );
    channel = std::stoi( row[3] ); speed = std::stoi( row[4] );
    privacy = strip( row[5] ); cipher = strip( row[6] ); authentification = strip( row[7] );
    power = std::stoi( row[8] ); nbr_beacons = std::stoi( row[9] ); nbr_IV = std::stoi( row[10] );
    lan_ip = strip( row[11] );
    id_length = std::stoi( row[12] ); ESSID = strip( row[13] );
    key = strip( remove_carriage_return( row[14] ) );
  }
}

std::string target_list::ap::serialize() const
{
  std::ostringstream result;
  
  result  << "{ "
          << "'BSSID':\"" << BSSID << "\", "
          << "'first_seen':\"" << first_seen << "\", "
          << "'last_seen':\"" << last_seen << "\", "
          << "'channel':" << channel << ", "
          << "'speed':" << speed << ", "
          << "'privacy':\"" << privacy << "\", "
          << "'cipher':\"" << cipher << "\", "
          << "'authentification':\"" << authentification << "\", "
          << "'power':" << power << ", "
          << "'beacons':" << nbr_beacons << ", "
          << "'IVs':" << nbr_IV << ", "
          << "'lan_ip':\"" << lan_ip << "\", "
          << "'id_length':" << id_length << ", "
          << "'ESSID':\"" << ESSID << "\", "
          << "'key':\"" << key << "\""
          << " }";
  
  return result.str();
}

target_list::station::station() {}

target_list::station::station( station& _station ) :  MAC( _station.MAC ), first_seen( _station.first_seen),
                                                      last_seen( _station.last_seen ), power( _station.power ),
                                                      nbr_packets( _station.nbr_packets ), BSSID( _station.BSSID ),
                                                      probed_BSSID( _station.probed_BSSID ), associated_ap(nullptr)
{}

target_list::station::station( CSVRow& row ) : associated_ap(nullptr)
{
  if( row.size() >= 7 )
  {
    MAC = strip( row[0] );
    first_seen = strip( row[1] );
    last_seen = strip( row[2] );
    power = std::stoi( row[3] );
    nbr_packets = std::stoi( row[4] );
    BSSID = strip( row[5] );
    probed_BSSID = strip( remove_carriage_return( row[6] ) );
  }
}

std::string target_list::station::serialize() const
{
  std::ostringstream result;
  
  result  << "{ "
          << "'MAC':\"" << MAC << "\", "
          << "'first_seen':\"" << first_seen << "\", "
          << "'last_seen':\"" << last_seen << "\", "
          << "'power':" << power << ", "
          << "'packets':" << nbr_packets << ", "
          << "'BSSID':\"" << BSSID << "\", "
          << "'probed_BSSID':\"" << probed_BSSID << "\""
          << " }";
  
  return result.str();
}

target_list::target_list()
{
  _aps      = new std::vector<ap*>();
  _stations = new std::vector<station*>();
}

target_list::~target_list()
{
  for(auto i : *_aps)
    delete i;
  delete _aps;
  
  for(auto i : *_stations)
    delete i;
  delete _stations;
}

const std::vector<target_list::ap*>& target_list::apList() const
{
  return *_aps;
}

const std::vector<target_list::station*>& target_list::stationList() const
{
  return *_stations;
}

void target_list::apList( const std::vector<ap*>& aps )
{
  mtx.lock();
  for(auto i : *_aps)
    delete i;
  _aps->clear();
  
  for(auto i : aps)
    _aps->push_back(i);
  mtx.unlock();
}

void target_list::stationList( const std::vector<station*>& stations )
{
  mtx.lock();
  for(auto i : *_stations)
    delete i;
  _stations->clear();
  
  for(auto i : stations)
    _stations->push_back(i);
  mtx.unlock();
}

const std::string target_list::serialize(const target_list::serialize_request req)
{
  mtx.lock();
  std::ostringstream rslt;
  
  //Only ALL is set for the moment
  //APS Things
  rslt  << "{'aps_size':" << _aps->size()
        << ", 'aps':[ ";
  for(auto i : *_aps)
    rslt << i->serialize();
  
  //STATIONS things
  rslt  << "], 'stations_size':" << _stations->size()
        << ", 'stations':[ ";
  for(auto i : *_stations)
    rslt << i->serialize();
  
  rslt << " ]}";
  
  mtx.unlock();
  return rslt.str();
}
