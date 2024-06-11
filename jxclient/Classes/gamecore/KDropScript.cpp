#include "KCore.h"
#include "KNpc.h"    //原来没有的
#include "KDropScript.h"
#include "LuaFuns.h"
#include "engine/KFilePath.h"
//#include "KDebug.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#else 
#include <io.h>
#include <direct.h>
#endif
//#include "Shlwapi.h"
