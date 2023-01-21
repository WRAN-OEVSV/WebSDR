/******************************************************************************
 * C++ source of RPX-100S
 *
 * File:   Socket.cpp
 * Author: Bernhard Isemann
 *
 * Created on 06 Jan 2022, 12:37
 * Updated on 07 Jan 2022, 17:00
 * Version 1.00
 *****************************************************************************/


#include "Socket.h"
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <iostream>


Socket::Socket() :
  m_sock ( -1 )
{

  memset ( &m_addr,
	   0,
	   sizeof ( m_addr ) );

}

Socket::~Socket()
{
  if ( is_valid() )
    ::close ( m_sock );
}

bool Socket::create()
{
  m_sock = socket ( PF_INET6,
		    SOCK_STREAM,
		    0 );

  if ( ! is_valid() )
    return false;


  // TIME_WAIT - argh
  int on = 1;
  if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
    return false;
  // this would make it IPv6 only. Maybe support it via an option.
  // setsockopt(m_sock, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));


  return true;

}



bool Socket::bind ( const int port )
{

  if ( ! is_valid() )
    {
      return false;
    }



  m_addr.sin6_family = AF_INET6;
  m_addr.sin6_addr = IN6ADDR_ANY_INIT;
  m_addr.sin6_port = htons ( port );

  int bind_return = ::bind ( m_sock,
			     ( struct sockaddr * ) &m_addr,
			     sizeof ( m_addr ) );


  if ( bind_return == -1 )
    {
      return false;
    }

  return true;
}


bool Socket::listen() const
{
  if ( ! is_valid() )
    {
      return false;
    }

  int listen_return = ::listen ( m_sock, MAXCONNECTIONS );


  if ( listen_return == -1 )
    {
      return false;
    }

  return true;
}


bool Socket::accept ( Socket& new_socket ) const
{
  int addr_length = sizeof ( m_addr );
  new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

  if ( new_socket.m_sock <= 0 )
    return false;
  else
    return true;
}


bool Socket::send ( const std::string s ) const
{
  auto status = ::send ( m_sock, s.c_str(), s.size(), MSG_NOSIGNAL );
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}


ssize_t Socket::recv ( std::string& s ) const
{
  char buf [ MAXRECV + 1 ];

  s = "";

  memset ( buf, 0, MAXRECV + 1 );

  auto status = ::recv ( m_sock, buf, MAXRECV, 0 );

  if ( status == -1 )
    {
      std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
      return 0;
    }
  else if ( status == 0 )
    {
      return 0;
    }
  else
    {
      s = buf;
      return status;
    }
}



bool Socket::connect ( const std::string &host, const int port )
{
  if ( ! is_valid() ) return false;

  m_addr.sin6_family = AF_INET6;
  m_addr.sin6_port = htons ( port );

  int status = inet_pton ( AF_INET6, host.c_str(), &m_addr.sin6_addr );

  if ( errno == EAFNOSUPPORT ) return false;

  status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );

  if ( status == 0 )
    return true;
  else
    return false;
}

void Socket::set_non_blocking ( const bool b )
{

  int opts;

  opts = fcntl ( m_sock,
		 F_GETFL );

  if ( opts < 0 )
    {
      return;
    }

  if ( b )
    opts = ( opts | O_NONBLOCK );
  else
    opts = ( opts & ~O_NONBLOCK );

  fcntl ( m_sock,
	  F_SETFL,opts );

}
