#include "utilities.hpp"

std::string strip( const std::string in )
{
  std::size_t k, v;
  for( k = 0; in[k] == ' ' && k < in.size(); k++);
  for( v = in.size()-1; in[v] == ' ' && v != 0; v--);
  
  return in.substr(k,v-k+1);
}

std::string remove_carriage_return( const std::string in )
{
  if( in.empty() )
    return in;
  
  if( in[ in.size() - 1 ] == '\r' )
    return in.substr(0, in.size() - 1);
  else
    return in;
}