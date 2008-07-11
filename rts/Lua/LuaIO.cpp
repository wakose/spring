#include "StdAfx.h"
// LuaIO.cpp: implementation of the LuaIO class.
//
//////////////////////////////////////////////////////////////////////

#include "LuaIO.h"

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string>
using std::string;

#include "LuaHandle.h"


/******************************************************************************/
/******************************************************************************/

static bool IsSafePath(const string& path)
{
	// keep searches within the Spring directory
	if ((path[0] == '/') || (path[0] == '\\') ||
	    ((path.size() >= 2) && (path[1] == ':'))) {
		return false;
	}
	if (path.find("..") != string::npos) {
		return false;
	}
	return true;
}


/******************************************************************************/
/******************************************************************************/

bool LuaIO::IsSimplePath(const string& path)
{
	// keep searches within the Spring directory
	if ((path[0] == '/') || (path[0] == '\\') ||
	    ((path.size() >= 2) && (path[1] == ':'))) {
		return false;
	}
	if (path.find("..") != string::npos) {
		return false;
	}
	return true;
}


bool LuaIO::SafeExecPath(const string& path)
{
	return true;
}


bool LuaIO::SafeReadPath(const string& path)
{
	return IsSimplePath(path);
}


bool LuaIO::SafeWritePath(const string& path)
{
	string prefix = ""; // FIXME
#if !defined UNITSYNC && !defined DEDICATED
	const CLuaHandle* lh = CLuaHandle::GetActiveHandle();
	if (lh != NULL) {
		prefix = lh->GetName() + "/" + "write";
	}
#endif
	if (path.compare(0, prefix.size(), prefix) != 0) {
		return false;
	}
	return true;
}


/******************************************************************************/
/******************************************************************************/

FILE* LuaIO::fopen(lua_State* L, const char* path, const char* mode)
{
	bool read  = false;
	bool write = false;

	// check the mode string
	const string modeStr = StringToLower(mode);
	if (modeStr.find_first_not_of("rwabt+") != string::npos) {
		errno = EINVAL;
		return NULL;
	}
	if (!IsSafePath(path)) {
		errno = EPERM; //EACCESS?
		return NULL;
	}
	return ::fopen(path, mode);
}


FILE* LuaIO::popen(lua_State* L, const char* command, const char* type)
{
	// check the type string
	const string typeStr = StringToLower(type);
	if (typeStr.find_first_not_of("rw") != string::npos) {
		errno = EINVAL;
		return NULL;
	}
	errno = EINVAL;	
	return NULL;
}


int LuaIO::pclose(lua_State* L, FILE* stream)
{
	errno = ECHILD;	
	return -1;
}


int LuaIO::system(lua_State* L, const char* command)
{
	return -1;
}


int LuaIO::remove(lua_State* L, const char* pathname)
{
	if (!SafeWritePath(pathname)) {
		errno = EPERM; //EACCESS?
		return -1;
	}
	return ::remove(pathname);
}


int LuaIO::rename(lua_State* L, const char* oldpath, const char* newpath)
{
	if (!SafeWritePath(oldpath) ||
			!SafeWritePath(newpath)) {
		errno = EPERM; //EACCESS?
		return -1;
	}
	return ::rename(oldpath, newpath);
}


/******************************************************************************/
/******************************************************************************/
