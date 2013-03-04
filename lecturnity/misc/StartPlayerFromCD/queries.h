#ifndef __AUTOSTART_QUERIES__
#define __AUTOSTART_QUERIES__


bool FileExists(const char *fileName);
bool javaIsInstalled(std::string &javaCommand);
bool javaIsOnCD(const char *cdRoot, std::string &javaCommand);
bool playerIsInstalled(const char *cdRoot, std::string &playerCommand);
int  playerIsOnCD(const char *cdRoot, std::string &playerCommand);
bool playerSetupIsAvailable(const char *cdRoot, std::string &setupName);



#endif
