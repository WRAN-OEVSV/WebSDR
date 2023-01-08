/******************************************************************************
 * C++ source of RPX-100
 *
 * File:   SocketException.h
 * Author: Fabian Franz
 *
 * Created on 07 Jan 2023, 14:00
 * Updated on 07 Jan 2023, 14:00
 * Version 1.00
 *****************************************************************************/

#include "SocketException.h"

SocketException::SocketException(const std::string &arg) : runtime_error(arg) {

}
