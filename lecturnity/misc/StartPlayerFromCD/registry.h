#ifndef __AUTOSTART_REGISTRY__
#define __AUTOSTART_REGISTRY__


bool ReadJavaEntry(std::string &javaPath);
bool ReadPlayerEntry(std::string &javaPath);
bool ComparePlayerVersion(std::string playerVersion);

#endif
