#pragma once
#ifndef ERROR_H
#define ERROR_H

#include <iostream>

static const char* _MESSAGE_SEP_ = 
  "--------------------------------------------------------------------------------\n";

static const char* _ERROR_SEP_ = 
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
  
static const char* _WARNING_SEP_ = 
  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";


#define PRINT_ERROR( message ) do {\
  std::cerr << _ERROR_SEP_ \
    << "Error occured in File: " << __FILE__ << "\n"\
    << "                 line: " << __LINE__ << "\n"\
    << "             function: " << __FUNCTION__ << "\n"\
    << "Error message: \n" << "  " << message << "\n"\
    << _ERROR_SEP_ << std::endl;\
} while (0)


#define PRINT_WARNING( message ) do {\
  std::cerr << _WARNING_SEP_ \
    << "Warning occured in File: " << __FILE__ << "\n"\
    << "                   line: " << __LINE__ << "\n"\
    << "               function: " << __FUNCTION__ << "\n"\
    << "Warning message: \n" << "  " << message << "\n"\
    << _WARNING_SEP_ << std::endl;\
} while (0)


#define PRINT_MESSAGE( message ) do {\
  std::cerr << _MESSAGE_SEP_ \
    << "              In File: " << __FILE__ << "\n"\
    << "                 line: " << __LINE__ << "\n"\
    << "             function: " << __FUNCTION__ << "\n"\
    << "Note: \n" << "  " << message << "\n"\
    << _MESSAGE_SEP_ << std::endl;\
} while (0)




#endif // ERROR_H
