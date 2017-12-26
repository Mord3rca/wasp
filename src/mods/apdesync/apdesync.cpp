#include "apdesync.hpp"

apdesync::apdesync()
{
  _mdk3_blacklist.open("/tmp/mdk3_blacklist", std::fstream::out);
  
  _mdk3 = childs.spawn( MDK3_CMD );
}

apdesync::~apdesync()
{
  _mdk3_blacklist.close();
  delete _mdk3;
}

const std::string apdesync::getName() const
{
  return _name;
}

const std::string apdesync::getOpCode() const
{
  return "None";
}

void apdesync::execOpCode( unsigned int opcode, void* args )
{
  switch( opcode )
  {
    case 200:
      {
        std::string* str = static_cast<std::string*>(args);
        std::cout << "Adding " << *str << " to Desync list" << std::endl;
        _aps.push_back( *str );
        _mdk3_blacklist << *str << std::endl;
      }
      break;
    
    default:
      break;
  }
}

void apdesync::_rewrite_file()
{
  _mdk3_blacklist.seekp(0);
  
  for( auto i : _aps )
  {
    _mdk3_blacklist << i << std::endl;
  }
}

#ifdef WASP_DEBUG
const std::string apdesync::print() const
{
  return "In DEV";
}
#endif
