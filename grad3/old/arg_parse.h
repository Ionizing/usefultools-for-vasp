/*
this method was inspired by 
https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
https://www.gnu.org/savannah-checkouts/gnu/libc/manual/html_node/Getopt-Long-Option-Example.html#Getopt-Long-Option-Example
*/

/*
  char* getCmdOption(const char** begin,const char** end, char* option);
  bool cmdOptionExists(char** begin, char** end, char* option);


*/

#include "global.h"
#include <getopt.h>

void Parse(int argc, char** argv); 
void PrintHelp();
void PrintFileOpenError(const char* fname);
