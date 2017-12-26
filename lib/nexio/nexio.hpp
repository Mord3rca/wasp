#ifndef NEXIO_HPP
#define NEXIO_HPP

extern const char* nex_default_if;
extern const char* nex_lib_path;

#include <stdexcept>

extern "C"
{
  #include <dlfcn.h>
}

//Get rid of nexutil, call nex_ioctl instead !
class nexioctl
{
  //According to: ./utilities/dhdutil/include/wlioctl.h in nexmon github
  const int NEXIO_GET_MONITOR = 107;
  const int NEXIO_SET_MONITOR = 108;
  
  struct nexio
  {
    int type;
    struct ifreq *ifr;
    int sock_rx_ioctl;
    int sock_rx_frame;
    int sock_tx;
    unsigned int securitycookie;
  };
  
public:
  nexioctl();
  ~nexioctl();
  
  int   monitor(void);
  void  monitor(int);
private:
  void* _handler;
  
  struct nexio *_io;
  struct nexio* (*_init)(void);
  int (*_nex_ioctl)(struct nexio*, int, void*, int, bool);
};

#endif //NEXIO_HPP
