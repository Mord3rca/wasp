#include "sniffer.hpp"

sniffer::sniffer()
{
  _inotify = new inotify_watcher("/tmp", IN_CREATE | IN_MODIFY);
    _inotify->setCallback( inotify_sniff_callback );
    _inotify->start();
  
  _airodump = childs.spawn( AIRODUMP_CMD );
  _airodump->stdin() << ' '; _airodump->stdin().flush();
}

sniffer::~sniffer()
{
  delete _inotify;
  delete _airodump;
}

const std::string sniffer::getName( void ) const
{
  return _name;
}

const std::string sniffer::getOpCode( void ) const
{
  #define JSON_FORMAT_OPCODE_DESC(opcode, desc)   "{ 'opcode':" << opcode << ", 'desc':\"" << desc << "\" }"
  const int opcode_count = 1;
  std::ostringstream e;
  
  e << "{ 'opcode_num':" << opcode_count << ", ["         << std::endl
    << JSON_FORMAT_OPCODE_DESC(200, "Force list update")  << std::endl
    << "] }";
  
  return e.str();
}

void sniffer::execOpCode( unsigned int opcode, void* args )
{
  switch( opcode )
  {
    case 200:
      this->update();
      break;
    
    default:
      break;
  }
}

void sniffer::filename( const std::string _file )
{
  _filename=_file;
}

const std::string sniffer::filename( void ) const
{
  return _filename;
}

void sniffer::update()
{
  if( time(nullptr) < (_lastsync + update_delay) || _filename.empty())
    return;
  
  std::vector<target_list::ap*>       aps;
  std::vector<target_list::station*>  stations;
  //Wait to airodump to finish its write ops
  std::this_thread::sleep_for( std::chrono::milliseconds(15) );
  
  std::fstream file("/tmp/" + _filename);
  CSV csv(file);
  csv.ReadAll();
  file.close();
  
  int process =1;
  _lastsync = time(nullptr);
  
  if( csv.size() >= 3 )
  {
    for(std::size_t i = 2; i < csv.size(); i++)
    {
      if( csv[i].size() < 3 )
      {
        process++; i += 2;
      }
      
      switch( process )
      {
        case 1:
        {
          try
          {
            CSVRow row = csv[i];
            target_list::ap *tmp = new target_list::ap( row );
            if( tmp->ESSID.empty() )
              tmp->ESSID = "[hidden]";
            
            aps.push_back( tmp );
          }
          catch(std::exception &e)
          {
            std::cerr << "Line reading error: " << csv[i][0] << std::endl;
          }
        }
          break;
          
        case 2:
        {
          try
          {
            CSVRow row = csv[i];
            stations.push_back( new target_list::station( row ) );
          }
          catch(std::exception &e)
          {
            std::cerr << "Line Reading Error: " << csv[i][0] << std::endl;
          }
        }
          break;
      }
    }
  }
  
  _link_devices(aps, stations);
  
  list->apList      (aps);
  list->stationList (stations);
}

void sniffer::_link_devices( std::vector<target_list::ap*>& aps, std::vector<target_list::station*>& stations ) const
{
  for( auto k : stations )
  {
    if( k->BSSID != "(not associated)" )
    {
      for( auto v : aps )
      {
        if( k->BSSID == v->BSSID )
        {
          k->associated_ap = v;
          break;
        }
      }
    }
  }
}

void inotify_sniff_callback(int mask, const std::string filename)
{
  sniffer *sniff = static_cast<sniffer*>(current_mode);
  
  if( (mask & IN_CREATE) && filename.find("result-") != std::string::npos )
  {
    sniff->filename( filename );
    sniff->update();
  }
  else if( (mask & IN_MODIFY) && filename == sniff->filename() )
    sniff->update();
}

#ifdef WASP_DEBUG
const std::string sniffer::print( void ) const
{
  std::ostringstream rslt;
  
  rslt  << "--- ModInfo ---" << std::endl
        << "-- Name : " << _name << std::endl
        << "-- LastSync: " << _lastsync << std::endl
        << "-- Target File: " << _filename << std::endl
        << "-- Opcodes: " << std::endl << getOpCode() << std::endl;
  
  return rslt.str();
}
#endif
