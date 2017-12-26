#include "nexio.hpp"

//Unused, netlink stuff don't need it.
const char* nex_default_if = "wlan0";
const char* nex_lib_path   = "/opt/wasp/lib/libnexio.so";

nexioctl::nexioctl() : _handler(nullptr), _init(nullptr), _nex_ioctl(nullptr)
{
  _handler = dlopen(nex_lib_path, RTLD_LAZY);
  _init =       (struct nexio* (*)(void))
                  dlsym(_handler, "nex_init_netlink");
  _nex_ioctl =  (int (*)(struct nexio*, int, void*, int, bool))
                  dlsym(_handler, "nex_ioctl");
  
  _io = (*_init)();
}

nexioctl::~nexioctl()
{
  if(_handler)
    dlclose(_handler);
}

int nexioctl::monitor(void)
{
  int buf = -1;
  if( (*_nex_ioctl)(_io, NEXIO_GET_MONITOR, &buf, sizeof(int), false) < 0)
    throw std::runtime_error("[NEXIO] Get monitor failed.");
  return buf;
}

void nexioctl::monitor( int mode )
{
  if( (*_nex_ioctl)(_io, NEXIO_SET_MONITOR, &mode, sizeof(int), true) < 0)
    throw std::runtime_error("[NEXIO] Set monitor failed.");
}
