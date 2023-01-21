/******************************************************************************
 * C++ source of RPX-100S
 *
 * File:   SocketException.h
 * Author: Bernhard Isemann
 *
 * Created on 06 Jan 2022, 12:37
 * Updated on 07 Jan 2022, 17:00
 * Version 1.00
 *****************************************************************************/


#ifndef SocketException_class
#define SocketException_class

#include <stdexcept>
#include <string>
#include <utility>

class SocketException : public std::runtime_error
{
 public:
  explicit SocketException(const std::string &arg);
};

#endif
