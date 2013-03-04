#ifndef __AUTOSTART_REGISTRY__
#define __AUTOSTART_REGISTRY__

//#include <windows.h>
#include <string>

/*
typedef struct {
  char
      day[5],
      month[5],
      year[5];
  std::string
      major,
      minor,
      micro;
   char
      typ[5];
} regEntry;
*/

bool ReadJavaEntry(std::string &javaPath);
bool ReadPlayerEntry(std::string &javaPath);
bool ComparePlayerVersion(std::string playerVersion);

#endif
