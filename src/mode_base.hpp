#ifndef WASP_MODE_BASE_HPP
#define WASP_MODE_BASE_HPP

/*
 * A virtual class for mode etc
 * Can also be used for dynamic loading
 * --> Why not a plugin interface ?!
 */

#include <string>

class mode_base
{
public:
  mode_base(){};
  virtual ~mode_base(){};
  
  virtual const std::string getName( void ) const = 0;
  //Return supported command
  virtual const std::string getOpCode( void ) const = 0;
  //Execute a command (with args or not)
  virtual void execOpCode(unsigned int, void* = nullptr) = 0;
  
  #ifdef WASP_DEBUG
  virtual const std::string print( void ) const = 0;
  #endif
};

extern mode_base    *current_mode;

#endif //WASP_MODE_BASE_HPP
