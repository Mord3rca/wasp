#ifndef WASP_SERVER_HPP
#define WASP_SERVER_HPP

#include "main.hpp"
#include "http_server.hpp"

#include "utilities.hpp"

#include <array>

extern std::array< const std::string, 3 > mime_json;

extern http::server *serv;
void server_callback( const http::request &req, http::answer &ans );

bool is_mime_type_json( const std::string& );

#ifdef WASP_DEBUG
void serv_debug( std::ostream& );
#endif

#endif //WASP_SERVER_HPP
