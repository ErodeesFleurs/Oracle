#include "lua.hpp"
#include <string>

#ifndef _STARLUA_H_ //如果没有引入头文件file.h
#define _STARLUA_H_ //那就引入头文件file.h
std::pair<std::string,bool> lua_gettablestring(lua_State* L,std::string key) {
    lua_pushstring(L, key.c_str());
    lua_gettable(L, 1);
    std::string s = "";
    if (lua_type(L, -1) != LUA_TNIL) {
        s = lua_tostring(L, -1);
        lua_pop(L, 1);
        return { s,1 };
    }
    lua_pop(L, 1);
    return { s,0 };
}

std::pair<int, bool>  lua_gettableint(lua_State* L, std::string key) {
    lua_pushstring(L, key.c_str());
    lua_gettable(L, 1);
    int res = NULL;
    if (lua_type(L, -1) != LUA_TNIL) {
        res = static_cast<int>(lua_tonumber(L, -1));
        lua_pop(L, 1);
        return { res,1 };
    }
    lua_pop(L, 1);
    return { res,0 };
}

std::pair<float, bool>  lua_gettablefloat(lua_State* L, std::string key) {
    lua_pushstring(L, key.c_str());
    lua_gettable(L, 1);
    float res = NULL;
    if (lua_type(L, -1) != LUA_TNIL) {
        res = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);
        return { res,1 };
    }
    lua_pop(L, 1);
    return { res,0 };
}

std::pair<std::array<float,2>, bool>  lua_gettablearray(lua_State* L, std::string key) {
    lua_pushstring(L, key.c_str());
    lua_gettable(L, 1);
    std::array<float, 2> res = { 0,0 };
    if (lua_type(L, -1) != LUA_TNIL) {
        for (int i = 0; i < 2; i++) {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, -2);
            res[i] = static_cast<float>(lua_tonumber(L, - 1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
        return { res,1 };
    }
    lua_pop(L, 1);
    return { res,0 };
}

#endif