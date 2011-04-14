#pragma once

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "AVGSquadManager.h"

static int AddSquad(lua_State* L)
{
	std::string squaduid(luaL_checkstring(L, 1));
	std::string squadid(luaL_checkstring(L, 2));
	//Ϊ�������һ�Ӳ���
	bool re = AVGSquadManager::getSingleton().addSquad(squaduid,squadid, "GameData/GameData/StoryData/SquadData");
	lua_pushboolean(L, re);
	return 1;
}

static const struct luaL_Reg AVGLib[] =
{
	{"AddSquad",AddSquad},
	{NULL,NULL}
};