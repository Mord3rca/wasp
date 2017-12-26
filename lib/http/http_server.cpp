#include "http_server.hpp"

#ifndef HTTP_SERVER_NAME
  const std::string http::server_name = "Wasp Indev";
#else
  const std::string http::server_name = HTTP_SERVER_NAME;
#endif

http::server::server( int port ) : _port(port), _run(true)
{
  struct sockaddr_in serv_addr;
  _sd = socket(AF_INET, SOCK_STREAM, 0);
  if(_sd < 0)
    throw std::runtime_error("HTTP Server: socket() error.");
  
  bzero( &serv_addr, sizeof(serv_addr) );
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(_port);
  
  if( bind(_sd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) < 0 )
    throw std::runtime_error("HTTP Server: bind() error." );
  
  listen(_sd, 2); //Only NGINX will contact this internal server.
  
  struct timeval timeout;
    timeout.tv_sec  = 2;
    timeout.tv_usec = 0;
  
  setsockopt(_sd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  //setsockopt(_sd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

http::server::~server()
{
  close(_sd);
}

void http::server::setCallback( void(*func)( const struct request&, answer&) )
{
  if(func)
    _callback = func;
}

void http::server::start()
{
  _th = new std::thread(&http::server::_request_handler, this);
}

void http::server::stop()
{
  _run.store(false);
  if( _th )
  {
    _th->join();
    delete _th;
  }
}

void http::server::_request_handler()
{
  struct sockaddr_in cli_addr; socklen_t clilen = sizeof(cli_addr);
  int cli_sd;
  
  while(_run)
  {
    cli_sd = accept(_sd, (struct sockaddr*) &cli_addr, &clilen);
    if(cli_sd > 0)
    {
      __gnu_cxx::stdio_filebuf<char> *filebuf_in = new __gnu_cxx::stdio_filebuf<char>( cli_sd, std::ios::in );
      __gnu_cxx::stdio_filebuf<char> *filebuf_out = new __gnu_cxx::stdio_filebuf<char>( cli_sd, std::ios::out );
      std::istream client_stream_in(filebuf_in);
      std::ostream client_stream_out(filebuf_out);
      
      struct request  *req = new struct request;
      struct answer   *ans = new struct answer;
      
      _parse_request(*req, client_stream_in);
      _callback(*req, *ans);
      
      ans->send( client_stream_out );
      
      delete req;
      delete ans;
      delete filebuf_in;
      delete filebuf_out;
    }
    else
      if(errno != EAGAIN)
        throw std::runtime_error("HTTP Server: accept() error.");
  }
}

void http::server::_parse_request( struct request &req, std::istream &in)
{
  std::string buffer; std::getline(in, buffer);
  std::istringstream buf_stream(buffer);
  
  std::string tmp; buf_stream >> tmp;
  if( tmp == "GET" )
    req.method = request::HTTP_GET;
  else if( tmp == "POST" )
    req.method = request::HTTP_POST;
  else
    throw std::runtime_error("HTTP Server: request parsing error.");
  
  buf_stream >> req.path;
  
  //Read all headers
  do
  {
    std::pair<std::string, std::string> header;
    std::getline(in, buffer);
    
    std::string::size_type separator_pos = buffer.find(':');
    if( separator_pos == std::string::npos )
      break;
    
    header.first  = buffer.substr(0, separator_pos);
    header.second = buffer.substr(separator_pos+2, buffer.length() - separator_pos -3); //Don't take \r
    
    req.headers.insert(header);
  }
  while( buffer != "\r");
  
  //Now read packet content.
  int packet_size = (req.getHeader("Content-Length") != "" ? 
                      std::atoi(req.getHeader("Content-Length").c_str()) : 0);
  std::ostringstream pkg_content;
  while( packet_size > 0)
  {
    std::string tmp_str; std::getline(in, tmp_str);
    packet_size -= tmp_str.size();
    pkg_content << tmp_str;
  }
  req.content.str( pkg_content.str() );
}

http::answer::answer() : status(200)
{
  addHeader("Server", server_name);
}

void http::answer::addHeader( const std::string key, const std::string value)
{
  if( headers.find(key) != headers.end() )
    headers.erase(key);
  
  headers.insert( std::pair<const std::string, const std::string>(key, value) );
}

void http::answer::send( std::ostream &out )
{
  std::string status_str;
  switch( status )
  {
    case 200:
      status_str = " OK";
      break;
      
    case 404:
      status_str = " Not Found";
      break;
      
    default:
      status_str = " N/A";
  }
  
  addHeader("Content-Length", std::to_string( content.str().length() ) );
  out << "HTTP/1.1 " << status << status_str << "\r\n";
  for(auto i : headers)
  {
    out << std::get<0>(i) << ": " << std::get<1>(i) << "\r\n";
  }
  out << "\r\n";
  
  out << content.str();
  out.flush();
}

const std::string http::request::getHeader( const std::string key ) const
{
  for(auto i : headers)
    if( key == std::get<0>(i) )
      return std::get<1>(i);
  
  return "";
}
