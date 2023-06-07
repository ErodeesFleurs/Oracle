// dllmain.cpp : 定义 DLL 应用程序的入口点。
#define PRIVATE 0

#include "pch.h"

#include <string>
#include <cstdio>
#include <cstring>
#include <array>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <map>
#include<any>
#include<windows.h>
#include <fstream>
#include <ctime>
#include <list>
#include<thread>
#include<mutex>
#include <locale>
#include <codecvt>
#include <winternl.h>
#include <comdef.h>

#include "PolyHook/PolyHook.hpp"
#include "StarLib.h"
#include "StarLua.h"
#include "json11.hpp"

static uintptr_t base = (uintptr_t)GetModuleHandle(0);

void* worldClientPointer, * mainPlayerPointer, * playerEntityPointer, * teamClientPointer, * clientContextPointer;
void* entityMapPointer, * inventoryPanePointer, * chatPointer, * assetsPointer, * clientWeatherPointer;
void* universeClientPointer, * clientApplicationPointer;

bool teleportedByOtherPlayers = false;//玩家能否被队伍成员传送
bool showChatAnimation = false, showMenuAnimation = false;//是否显示聊天...和菜单图标
bool alwaysMaster = false;
bool chatFocused = false;//是否正在聊天栏输入信息
bool inventoryNetworked = false;//是否将背包物品上传服务器
bool ignorePickUpItems = false;//忽略物品拾取
bool ignorePhysicsObjectCollisions = false;//TODO: 忽略OBJ实体碰撞 不确定是否有用
bool ignoreProjectileCollisions = false;
bool ignoreVehicleCollisions = false;
bool reachEntities = false;//是否能穿过墙壁和实体交互/远程交互
std::string customPortrait = "";//自定义队伍头像
std::string spoofUuid = ""; //伪造UUID
std::string customChatMessage = "";//自定义聊天信息
std::string playerTechDirectives = "";
std::string playerStatusDirectives = "";
std::string teamName = "";

//部分函数

void JsonToTable(lua_State* L, json11::Json val) {
	switch (val.type()) {
	case json11::Json::NUL: {
		Star::Lua::pushNil(base + Star::Lua::PUSHNIL)(L);
		break;
	}
	case json11::Json::BOOL: {
		Star::Lua::pushBoolean(base + Star::Lua::PUSHBOOLEAN)(L, val.bool_value());
		break;
	}
	case json11::Json::NUMBER: {
		double intv = val.int_value(), dbv = val.number_value();
		if (intv == dbv) Star::Lua::pushInteger(base + Star::Lua::PUSHINTEGER)(L, intv);
		else Star::Lua::pushNumber(base + Star::Lua::PUSHNUMBER)(L, dbv);
		break;
	}
	case json11::Json::ARRAY: {
		Star::Lua::createTable(base + Star::Lua::CREATETABLE)(L, 0, 0);
		auto vt = val.array_items();
		for (int i = 0; i < vt.size(); i++) {
			JsonToTable(L, vt[i]);
			Star::Lua::rawSeti(base + Star::Lua::RAWSETI)(L, -2, i + 1);
		}
		break;
	}
	case json11::Json::STRING: {
		Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, (val.string_value()).c_str());
		break;
	}
	case json11::Json::OBJECT: {
		Star::Lua::createTable(base + Star::Lua::CREATETABLE)(L, 0, 0);
		for (auto& ve : val.object_items()) {
			Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, (ve.first).c_str());
			JsonToTable(L, ve.second);
			Star::Lua::rawSet(base + Star::Lua::RAWSET)(L, -3);
		}
		break;
	}
	}
}

int JsonForTable(lua_State* L, std::string& s) {
	std::string err;
	json11::Json j = json11::Json::parse(s, err);
	JsonToTable(L, j);
	return 1;
}

void dummyResponse(void* const&) { // invokeRemote 回调函数

}


//已证明hook可行性
Star::WorldClient::worldClient world_client_hookTramp;
std::shared_ptr<PLH::Detour> World_Client_Hook(new PLH::Detour);
void __fastcall WORLD_CLIENT_HOOK(void* wc, std::shared_ptr<void> mainPlayer) {
	world_client_hookTramp(wc, mainPlayer);
	worldClientPointer = wc;
	mainPlayerPointer = mainPlayer.get();
	int playerEntityOffset = 0x1450;
	playerEntityPointer = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + playerEntityOffset);
	return;
}

Star::TeamClient::teamClient team_client_hookTramp;
std::shared_ptr<PLH::Detour> Team_Client_Hook(new PLH::Detour);
void __fastcall TEAM_CLIENT_HOOK(void* tc, std::shared_ptr<void> mainPlayer, std::shared_ptr<void> clientContext) {
	teamClientPointer = tc;
	clientContextPointer = clientContext.get();
	team_client_hookTramp(tc, mainPlayer, clientContext);
	return;
}

Star::TeamClient::invokeRemote team_invoke_Remote_hookTramp;
std::shared_ptr<PLH::Detour> Team_Invoke_Remote_Hook(new PLH::Detour);
void __fastcall TEAM_INVOKE_REMOTE_HOOK(void* tc, std::string* method, void* args, std::function<void __cdecl(void* const&)> responseFunction) {
	std::string methodStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(method);
	if (methodStr == "team.updateStatus") {
		if (!teleportedByOtherPlayers) {
			void* warpModeJson = malloc(128);
			std::string warpMode = "None";
			Star::Json::JsonFromString(base + Star::Json::JSONFROMSTRING)(warpModeJson, &warpMode);
			std::string keyStr = "warpMode";
			Star::Json::setKey(base + Star::Json::SETKEY)(args, args, &keyStr, warpModeJson);
			free(warpModeJson);
		}
		if (teamName != "") {
			void* nameJson = malloc(128);
			std::string teamNameStr = teamName;
			Star::Json::JsonFromString(base + Star::Json::JSONFROMSTRING)(nameJson, &teamNameStr);
			std::string keyStr = "name";
			Star::Json::setKey(base + Star::Json::SETKEY)(args, args, &keyStr, nameJson);
			free(nameJson);
		}
		if (customPortrait != "") {
			std::string keyStr = "portrait";
			std::string portraitJsonStr = customPortrait;
			void* portraitJson = malloc(200 * 1024);//200KB大抵是够了
			Star::Json::parseJson(base + Star::Json::PARSEJSON)(portraitJson, &customPortrait);
			Star::Json::setKey(base + Star::Json::SETKEY)(args, args, &keyStr, portraitJson);
			free(portraitJson);
		}
	}
	team_invoke_Remote_hookTramp(tc, method, args, responseFunction);
	return;
}

Star::EntityMap::entityQuery entity_Map_Entity_Query_hookTramp;
std::shared_ptr<PLH::Detour> Entity_Map_Entity_Query_Hook(new PLH::Detour);
void* __fastcall ENTITY_MAP_ENTITY_QUERY_HOOK(void* em, void* result, void* boundBox, void* filter) {
	entityMapPointer = em;
	return entity_Map_Entity_Query_hookTramp(em, result, boundBox, filter);
}

Star::Entity::isMaster entity_Is_Master_hookTramp;
std::shared_ptr<PLH::Detour> Entity_Is_Master_Hook(new PLH::Detour);
bool __fastcall ENTITY_IS_MASTER_HOOK(void* e) {
	bool res = entity_Is_Master_hookTramp(e);
	if (alwaysMaster) res = true;
	return res;
}

Star::Player::setBusyState player_Set_Busy_State_hookTramp;
std::shared_ptr<PLH::Detour> Player_Set_Busy_State_Hook(new PLH::Detour);
void PLAYER_SET_BUSY_STATE_HOOK(void* pi, int index) {
	if (pi == mainPlayerPointer) {
		//define 0: chat  1: meun 2
		if (index == 1 && !showChatAnimation) return;
		else if (index == 2 && !showMenuAnimation) return;
	}
	player_Set_Busy_State_hookTramp(pi, index);
	return;
}

Star::InventoryPane::update inventory_Pane_Update_hookTramp;
std::shared_ptr<PLH::Detour> Inventory_Pane_Update_Hook(new PLH::Detour);
void INVENTORY_PANE_UPDATE_HOOK(void* pi) {
	inventoryPanePointer = pi;
	inventory_Pane_Update_hookTramp(pi);
	return;
}

Star::Player::netStore player_Net_Store_hookTramp;
std::shared_ptr<PLH::Detour> Player_Net_Store_Hook(new PLH::Detour);
void* PLAYER_NET_STORE_HOOK(void* p, void* res) {
	void* maybeStr = malloc(1024 * 20);
	void* maybeStrTemp = malloc(1024 * 20);
	if (spoofUuid != "") {
		std::string temp = spoofUuid;
		Star::Entity::uniqueId(base + Star::Entity::UNIQUEID)(playerEntityPointer, maybeStrTemp);
		Star::Maybe::String::maybe(base + Star::Maybe::String::MAYBE)(maybeStr, &temp);
		Star::Entity::setUniqueId(base + Star::Entity::SETUNIQUEID)(playerEntityPointer, maybeStr);
	}
	void* tmp = player_Net_Store_hookTramp(p, res);
	if (spoofUuid != "") {
		Star::Entity::setUniqueId(base + Star::Entity::SETUNIQUEID)(playerEntityPointer, maybeStrTemp);
	}
	free(maybeStr);
	free(maybeStrTemp);
	return tmp;
}

Star::Uuid::opequal uuid_Opequal_hookTramp;
std::shared_ptr<PLH::Detour> Uuid_Opequal_Hook(new PLH::Detour);
bool UUID_OPEQUAL_HOOK(void* u, void* ut) {
	return true;
}

Star::Chat::chat chat_Chat_hookTramp = NULL;
std::shared_ptr<PLH::Detour> Chat_Chat_Hook(new PLH::Detour);
void CHAT_CHAT_HOOK(void* c, std::shared_ptr<void> client) {
	chatPointer = c;
	chat_Chat_hookTramp(c, client);
	return;
}

Star::TechController::parentDirectives tech_Controller_Parent_Directives_hookTramp;
std::shared_ptr<PLH::Detour> Tech_Controller_Parent_Directives_Hook(new PLH::Detour);
void* TECH_CONTROLLER_PARENT_DIRECTIVES_HOOK(void* tc, void* result) {
	void* dirStar = tech_Controller_Parent_Directives_hookTramp(tc, result);
	playerTechDirectives = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(dirStar);
	return dirStar;
}

Star::StatusController::parentDirectives status_Controller_Parent_Directives_hookTramp;
std::shared_ptr<PLH::Detour> Status_Controller_Parent_Directives_Hook(new PLH::Detour);
void* STATUS_CONTROLLER_PARENT_DIRECTIVES_HOOK(void* sc, void* result) {
	void* dirStar = status_Controller_Parent_Directives_hookTramp(sc, result);
	playerStatusDirectives = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(dirStar);
	return dirStar;
}

Star::Assets::assets assets_Assets_hookTramp;
std::shared_ptr<PLH::Detour> Assets_Assets_Hook(new PLH::Detour);
void ASSETS_ASSETS_HOOK(void* as, void* settings, void* assetSources) {
	assetsPointer = as;
	assets_Assets_hookTramp(as, settings, assetSources);
	return;
}

Star::ClientWeather::clientWeather client_Weather_Client_Weather_hookTramp;
std::shared_ptr<PLH::Detour> Client_Weather_Client_Weathe_Hook(new PLH::Detour);
void CLIENT_WEATHER_CLIENT_WEATHER_HOOK(void* cw) {
	clientWeatherPointer = cw;
	client_Weather_Client_Weather_hookTramp(cw);
	return;
}

Star::UniverseClient::sendChat universe_Client_Send_Chat_hookTramp;
std::shared_ptr<PLH::Detour> Universe_Client_Send_Chat_Hook(new PLH::Detour);
void UNIVERSE_CLIENT_SEND_CHAT_HOOK(void* uc, void* text, Star_ChatSendMode sendMode) {
	if (customChatMessage == "") {
		universe_Client_Send_Chat_hookTramp(uc, text, sendMode);
	}
	else {
		universe_Client_Send_Chat_hookTramp(uc, &customChatMessage, sendMode);
		customChatMessage = "";
	}
	return;
}

Star::UniverseClient::universeClient universe_Client_Universe_Client_hookTramp;
std::shared_ptr<PLH::Detour> Universe_Client_Universe_Client_Hook(new PLH::Detour);
void UNIVERSE_CLIENT_UNIVERSE_CLIENT_HOOK(void* uc, std::shared_ptr<void> playerStorage, std::shared_ptr<void> statistics) {
	universeClientPointer = uc;
	universe_Client_Universe_Client_hookTramp(uc, playerStorage, statistics);
	return;
}

Star::PlayerInventory::netElementsNeedStore player_Inventory_Net_Elements_Need_Store_hookTramp;
std::shared_ptr<PLH::Detour> Player_Inventory_Net_Elements_Need_Store_Hook(new PLH::Detour);
void PLAYER_INVENTORY_NET_ELEMENTS_NEED_STORE_HOOK(void* pi) {
	if (inventoryNetworked) {
		player_Inventory_Net_Elements_Need_Store_hookTramp(pi);
	}
	return;
}

Star::PlayerInventory::itemsCanFit player_Inventory_Items_Can_Fit_hookTramp;
std::shared_ptr<PLH::Detour> Player_Inventory_Items_Can_Fit_Hook(new PLH::Detour);
unsigned __int64 PLAYER_INVENTORY_ITEMS_CAN_FIT_HOOK(void* pi, std::shared_ptr<void>* items) {
	if (!ignorePickUpItems)  return player_Inventory_Items_Can_Fit_hookTramp(pi, items);
	else return 0;
}


Star::WorldClient::canReachEntity world_Client_Can_Reach_Entity_hookTramp;
std::shared_ptr<PLH::Detour> World_Client_Can_Reach_Entity_Hook(new PLH::Detour);
bool WORLD_CLIENT_CAN_REACH_ENTITY_HOOK(void* wc, std::array<float, 2>* position, float radius, int targetEntity, bool preferInteractive) {
	if (reachEntities)return 1;
	int inWorldOffset = 0x6F9, geometryOffset = 0x178, tileArrayOffset = 0x148, entityMapOffset = 0x138;
	bool inWorld = reinterpret_cast<bool>(reinterpret_cast<char*>(wc) + inWorldOffset);
	void* geometry = reinterpret_cast<void*>(reinterpret_cast<char*>(wc) + geometryOffset);
	void* tileArray = reinterpret_cast<void*>(reinterpret_cast<char*>(wc) + tileArrayOffset);
	void* entityMap = reinterpret_cast<void*>(reinterpret_cast<char*>(wc) + entityMapOffset);
	bool impl = Star::WorldImpl::canReachEntity(base + Star::WorldImpl::CANREACHENTITY)(geometry, tileArray, entityMap, position, radius, targetEntity, preferInteractive);
	return inWorld && impl;
}

Star::PhysicsObject::movingCollisionCount physics_Object_Moving_Collision_Count_hookTramp;
std::shared_ptr<PLH::Detour> Physics_Object_Moving_Collision_Count_Hook(new PLH::Detour);
unsigned __int64 PHYSICS_OBJECT_MOVING_COLLISION_COUNT_HOOK(void* po) {
	if (ignorePhysicsObjectCollisions)  return 0;
	return physics_Object_Moving_Collision_Count_hookTramp(po);
}

Star::Projectile::movingCollisionCount projectile_Moving_Collision_Count_hookTramp;
std::shared_ptr<PLH::Detour> Projectile_Moving_Collision_Count_Hook(new PLH::Detour);
void* PROJECTILE_MOVING_COLLISION_COUNT_HOOK(void* po) {
	if (ignoreProjectileCollisions)  return nullptr;
	return projectile_Moving_Collision_Count_hookTramp(po);
}

Star::Projectile::movingCollisionCount vehicle_Moving_Collision_Count_hookTramp;
std::shared_ptr<PLH::Detour> Vehicle_Moving_Collision_Count_Hook(new PLH::Detour);
void* VEHICLE_MOVING_COLLISION_COUNT_HOOK(void* po) {
	if (ignoreVehicleCollisions)  return nullptr;
	return vehicle_Moving_Collision_Count_hookTramp(po);
}

EXTERN_C int setName(lua_State* L) {
	std::string s = lua_tostring(L, 1);
	Star::Player::setName(base + Star::Player::SETNAME)(mainPlayerPointer, &s);
	return 0;
}

EXTERN_C int setGender(lua_State* L) {
	bool gender = lua_toboolean(L, 1);
	Star::Player::setGender(base + Star::Player::SETGENDER)(mainPlayerPointer, &gender);
	return 0;
}

EXTERN_C int setSpecies(lua_State* L) {
	std::string s = lua_tostring(L, 1);
	if (s != "apex" && s != "avian" && s != "floran" && s != "glitch" && s != "human" && s != "hylotl" && s != "penguin") {
		Star::Logger::log(base + Star::Logger::LOG)(Star::LogLevel::Error, "Oracle setSpecies ERROR: The value of species is not a correct species");
		return 0;
	}
	Star::Player::setSpecies(base + Star::Player::SETSPECIES)(mainPlayerPointer, &s);
	return 0;
}

EXTERN_C int setPersonality(lua_State* L) {
	int idleOffset = 0x6F8 + 0x1A8, armIdleOffset = 0x6F8 + 0x1A8 + 0x20;
	int headOffset = 0x6F8 + 0x1A8 + 0x40, armOffset = 0x6F8 + 0x1A8 + 0x48;

	void* idlePtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + idleOffset);
	void* armIdlePtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + armIdleOffset);

	std::string idletStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(idlePtr);
	std::string armIdleStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(armIdlePtr);
	std::array<float, 2>* headOffsetArr = reinterpret_cast<std::array<float, 2>*>(reinterpret_cast<char*>(mainPlayerPointer) + headOffset);
	std::array<float, 2>* armOffsetArr = reinterpret_cast<std::array<float, 2>*>(reinterpret_cast<char*>(mainPlayerPointer) + armOffset);
	std::pair<std::string, bool> idleState = lua_gettablestring(L, "idle");
	std::pair<std::string, bool> armIdleState = lua_gettablestring(L, "armIdle");
	std::pair<std::array<float, 2>, bool> headOffsetState = lua_gettablearray(L, "headOffset");
	std::pair<std::array<float, 2>, bool> armOffsetState = lua_gettablearray(L, "armOffset");
	if (idleState.second) idletStr = idleState.first;
	if (armIdleState.second) armIdleStr = armIdleState.first;
	if (headOffsetState.second) headOffsetArr = &headOffsetState.first;
	if (armOffsetState.second) armOffsetArr = &armOffsetState.first;

	std::string personalityJsonStr = "[\"" + idletStr + "\",\"" + armIdleStr + "\",[" + std::to_string((*headOffsetArr)[0]) + "," + std::to_string((*headOffsetArr)[1]) + "],[" + std::to_string((*armOffsetArr)[0]) + "," + std::to_string((*armOffsetArr)[1]) + "]]";
	void* personalityJson = malloc(2 * 1024);
	void* personalityPtr = malloc(2 * 1024);
	Star::Json::parseJson(base + Star::Json::PARSEJSON)(personalityJson, &personalityJsonStr);
	Star::parsePersonality(base + Star::PARSEPERSONALITY)(personalityPtr, personalityJson);
	Star::Player::setPersonality(base + Star::Player::SETPERSONALITY)(mainPlayerPointer, personalityPtr);
	free(personalityJson);
	free(personalityPtr);
	return 0;
}

EXTERN_C int setTeamPortrait(lua_State* L) {
	lua_pushnil(L);
	std::string str = "";
	str += "[";
	while (lua_next(L, 1)) {
		str += "{";
		int cnt = lua_gettop(L);
		lua_pushnil(L);
		while (lua_next(L, cnt)) {
			std::string key = lua_tostring(L, -2);
			int val = lua_type(L, -1);
			if (lua_type(L, -1) == LUA_TBOOLEAN && key == "fullbright") {
				str += "\"fullbright\":";
				if (lua_toboolean(L, -1) == 0) str += "false,";
				else str += "true,";
			}
			else if (val == LUA_TSTRING && key == "image") {
				str += "\"image\":";
				str += "\"" + (std::string)lua_tostring(L, -1) + "\",";
			}
			else if (val == LUA_TTABLE && key == "position") {
				str += "\"position\":";
				str += "[";
				int count = 0;
				int cntPos = lua_gettop(L);
				lua_pushnil(L);
				while (lua_next(L, cntPos)) {
					if (count < 2) str += std::to_string(lua_tonumber(L, -1)) + ",";
					count++;
					lua_pop(L, 1);
				}
				str.pop_back();
				str += "],";
			}
			else if (val == LUA_TNUMBER && key == "scale") {
				str += "\"scale\" : ";
				str += std::to_string(lua_tonumber(L, -1)) + ",";
			}
			lua_pop(L, 1);
		}
		str.pop_back();
		lua_pop(L, 1);
		str += "},";
	}
	str.pop_back();
	str += "]";
	customPortrait = str;
	return 0;
}

EXTERN_C int joinParty(lua_State* L) {
	std::string s = lua_tostring(L, 1);
	void* uuidPtr = malloc(s.size() * 4 * sizeof(wchar_t));
	if (teamClientPointer != nullptr) {
		Star::Uuid::uuid(base + Star::Uuid::UUID)(uuidPtr, &s);
		Star::TeamClient::acceptInvitation(base + Star::TeamClient::ACCEPTINVITATION)(teamClientPointer, uuidPtr);
	}
	free(uuidPtr);
	return 0;
}

Star::Json::parseJson test_Tramp = NULL;
std::shared_ptr<PLH::Detour> Test_Hook(new PLH::Detour);
void TEST_HOOK(void* res, void* json) {
	std::string tmp = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(json);
	Star::Logger::log(base + Star::Logger::LOG)(Star::Info, tmp.c_str());
	return;
}

EXTERN_C int  invitePlayer(lua_State* L) {
	int cnum = lua_gettop(L);
	int nameOffset = 0x6F8, uuidOffset = 0x18;
	std::string inviteeName = lua_tostring(L, 1);
	void* inviterNamePtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + nameOffset);
	std::string inviterName = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(inviterNamePtr);
	if (cnum >= 2)  inviterName = lua_tostring(L, 2);
	void* inviterUuidPtr = reinterpret_cast<Star::MaybeDS<Star::StringDS>*>(reinterpret_cast<char*>(playerEntityPointer) + uuidOffset);
	std::string inviterUuid = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(inviterUuidPtr);
	if (cnum >= 3) inviterUuid = lua_tostring(L, 3);

	std::string methodName = "team.invite";
	std::string argsJsonStr = "{\"inviteeName\":\"" + inviteeName + "\",\"inviterName\":\"" + inviterName + "\",\"inviterUuid\":\"" + inviterUuid + "\"}";

	void* argsJson = malloc(1024 * 1024 * 512); //512 TO SPAM
	Test_Hook->SetupHook((BYTE*)(base + Star::Json::PARSEJSON), (BYTE*)&TEST_HOOK);
	Star::Json::parseJson(base + Star::Json::PARSEJSON)(argsJson, &argsJsonStr);

	std::function<void __cdecl(void* const&)> responseFunction = dummyResponse;
	Star::TeamClient::invokeRemote(base + Star::TeamClient::INVOKEREMOTE)(teamClientPointer, &methodName, argsJson, responseFunction);

	free(argsJson);
	return 0;
}

EXTERN_C int removeEntity(lua_State* L) {
	int entityID = static_cast<int>(lua_tonumber(L, 1));
	alwaysMaster = true;
	std::shared_ptr<void> resultPtr;
	Star::EntityMap::removeEntity(base + Star::EntityMap::REMOVEENTITY)(entityMapPointer, &resultPtr, entityID);
	Star::WorldClient::removeEntity(base + Star::WorldClient::REMOVEENTITY)(worldClientPointer, entityID, true);
	alwaysMaster = false;
	return 0;
}

EXTERN_C int suicide(lua_State* L) {
	Star::Player::kill(base + Star::Player::KILL)(mainPlayerPointer);
	return 0;
}

EXTERN_C int setBodyDirectives(lua_State* L) {
	std::string newDirectives = lua_tostring(L, 1);
	Star::Player::setBodyDirectives(base + Star::Player::SETBODYDIRECTIVES)(mainPlayerPointer, &newDirectives);
	return 0;
}

EXTERN_C int setEmoteDirectives(lua_State* L) {
	std::string newDirectives = lua_tostring(L, 1);
	Star::Player::setEmoteDirectives(base + Star::Player::SETEMOTEDIRECTIVES)(mainPlayerPointer, &newDirectives);
	return 0;
}

EXTERN_C int setHairDirectives(lua_State* L) {
	std::string newDirectives = lua_tostring(L, 1);
	Star::Player::setHairDirectives(base + Star::Player::SETHAIRDIRECTIVES)(mainPlayerPointer, &newDirectives);
	return 0;
}

EXTERN_C int setHairType(lua_State* L) {
	std::string group = lua_tostring(L, 1);
	std::string type = lua_tostring(L, 2);
	Star::Player::setHairType(base + Star::Player::SETHAIRTYPE)(mainPlayerPointer, &group, &type);
	return 0;
}

EXTERN_C int setFacialHair(lua_State* L) {
	int groupOffset = 0x6F8 + 0xE8, typeOffset = 0x6F8 + 0x108, directivesOffset = 0x6F8 + 0x128;

	void* groupOffsetStrPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + groupOffset);
	void* typeOffsetStrPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + typeOffset);
	void* directivesStrPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + directivesOffset);

	std::string groupStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(groupOffsetStrPtr);
	std::string typeStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(typeOffsetStrPtr);
	std::string directivesStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(directivesStrPtr);

	std::pair<std::string, bool> groupState = lua_gettablestring(L, "group");
	std::pair<std::string, bool> typeState = lua_gettablestring(L, "type");
	std::pair<std::string, bool> directivesState = lua_gettablestring(L, "directives");
	if (groupState.second) groupStr = groupState.first;
	if (typeState.second) typeStr = typeState.first;
	if (directivesState.second) directivesStr = directivesState.first;
	Star::Player::setFacialHair(base + Star::Player::SETFACIALHAIR)(mainPlayerPointer, &groupStr, &typeStr, &directivesStr);
	return 0;
}

EXTERN_C int setFacialMask(lua_State* L) {
	int groupOffset = 0x6F8 + 0x148, typeOffset = 0x6F8 + 0x168, directivesOffset = 0x6F8 + 0x188;

	void* groupOffsetStrPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + groupOffset);
	void* typeOffsetStrPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + typeOffset);
	void* directivesStrPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + directivesOffset);

	std::string groupStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(groupOffsetStrPtr);
	std::string typeStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(typeOffsetStrPtr);
	std::string directivesStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(directivesStrPtr);

	std::pair<std::string, bool> groupState = lua_gettablestring(L, "group");
	std::pair<std::string, bool> typeState = lua_gettablestring(L, "type");
	std::pair<std::string, bool> directivesState = lua_gettablestring(L, "directives");
	if (groupState.second) groupStr = groupState.first;
	if (typeState.second) typeStr = typeState.first;
	if (directivesState.second) directivesStr = directivesState.first;
	Star::Player::setFacialMask(base + Star::Player::SETFACIALMASK)(mainPlayerPointer, &groupStr, &typeStr, &directivesStr);
	return 0;
}

EXTERN_C int forceNude(lua_State* L) {
	Star::Player::forceNude(base + Star::Player::FORCENUDE)(mainPlayerPointer);
	return 0;
}

EXTERN_C int lounge(lua_State* L) {
	int entityid = static_cast<int>(lua_tonumber(L, 1));
	unsigned __int64 anchorIndex = static_cast<unsigned __int64>(lua_tonumber(L, 2));
	Star::Player::lounge(base + Star::Player::LOUNGE)(mainPlayerPointer, entityid, anchorIndex);
	return 0;
}

EXTERN_C int setColor(lua_State* L) {
	int count = lua_gettop(L);
	char r = static_cast<char>(lua_tonumber(L, 1));
	char g = static_cast<char>(lua_tonumber(L, 2));
	char b = static_cast<char>(lua_tonumber(L, 3));
	char a = (char)255;
	if (count == 4) {
		a = static_cast<char>(lua_tonumber(L, 4));
	}
	int colorOffset = 0x6F8 + 0x1F8;
	std::array<char, 4>* colorPtr = reinterpret_cast<std::array<char, 4>*>(reinterpret_cast<char*>(mainPlayerPointer) + colorOffset);
	*colorPtr = { r,g,b,a };
	return 0;
}

EXTERN_C int setEmoteState(lua_State* L) {
	int stateT = static_cast<int>(lua_tonumber(L, 1));
	int humanoidPtrOffset = 0x138;
	void* humanoidPtr = (reinterpret_cast<void*>(reinterpret_cast<char*>(mainPlayerPointer) + humanoidPtrOffset));
	Star::Humanoid::setEmoteState(base + Star::Humanoid::SETEMOTESTATE)(humanoidPtr, (Star::HumanoidEmote)stateT);
	return 0;
}

EXTERN_C int setChatAnimation(lua_State* L) {
	showChatAnimation = lua_toboolean(L, 1);
	return 0;
}

EXTERN_C int setMeunAnimation(lua_State* L) {
	showMenuAnimation = lua_toboolean(L, 1);
	return 0;
}

EXTERN_C int setCanTp(lua_State* L) {
	teleportedByOtherPlayers = lua_toboolean(L, 1);
	return 0;
}

EXTERN_C int setSpoofUniqueID(lua_State* L) {
	spoofUuid = lua_tostring(L, 1);
	return 0;
}

EXTERN_C int removeFromTeam(lua_State* L) {
	std::string uuidString = lua_tostring(L, 1);
	void* uuidPtr = malloc(uuidString.size() * 2 * sizeof(wchar_t));
	Star::Uuid::uuid(base + Star::Uuid::UUID)(uuidPtr, &uuidString);
	Uuid_Opequal_Hook->SetupHook((BYTE*)(base + Star::Uuid::OPEQUAL), (BYTE*)&UUID_OPEQUAL_HOOK);
	Uuid_Opequal_Hook->Hook();
	Star::TeamClient::removeFromTeam(base + Star::TeamClient::REMOVEFROMTEAM)(teamClientPointer, uuidPtr);
	Uuid_Opequal_Hook->UnHook();
	free(uuidPtr);
	return 0;
}

EXTERN_C int playerAimPosition(lua_State* L) {
	int inp = static_cast<int>(lua_tonumber(L, 1));
	std::shared_ptr<void> resultPtr;
	Star::EntityMap::getPlayer(base + Star::EntityMap::GETPLAYER)(entityMapPointer, &resultPtr, inp);
	void* playerPtr = resultPtr.get();
	std::array<float, 2> aposPtr;
	//Star::Player::aimPosition(base + Star::Player::AIMPOSITION)(playerPtr, &aposPtr);
	int aimPositionOffset = 0x664;
	aposPtr = *(reinterpret_cast<std::array<float, 2>*>(reinterpret_cast<char*>(playerPtr) + aimPositionOffset));
	Star::Lua::createTable(base + Star::Lua::CREATETABLE)(L, 0, 0);
	Star::Lua::pushInteger(base + Star::Lua::PUSHINTEGER)(L, 1);
	Star::Lua::pushNumber(base + Star::Lua::PUSHNUMBER)(L, aposPtr[0]);
	Star::Lua::setTable(base + Star::Lua::SETTABLE)(L, -3);
	Star::Lua::pushInteger(base + Star::Lua::PUSHINTEGER)(L, 2);
	Star::Lua::pushNumber(base + Star::Lua::PUSHNUMBER)(L, aposPtr[1]);
	Star::Lua::setTable(base + Star::Lua::SETTABLE)(L, -3);
	return 1;
}

EXTERN_C int  savePlayerFile(lua_State* L) {
	int inp = static_cast<int>(lua_tonumber(L, 1));
	std::shared_ptr<void> resultPtr;
	Star::EntityMap::getPlayer(base + Star::EntityMap::GETPLAYER)(entityMapPointer, &resultPtr, inp);
	void* playerPtr = resultPtr.get();
	if (playerPtr == nullptr) {
		Star::Lua::pushNil(base + Star::Lua::PUSHNIL)(L);
		return 1;
	}
	void* playerJson = malloc(sizeof(unsigned char) * 1024 * 1024 * 20);
	Star::Player::diskStore(base + Star::Player::DISKSTORE)(playerPtr, playerJson);
	void* jsonStarString = malloc(sizeof(unsigned char) * 1024 * 1024 * 20);
	Star::Json::printJson(base + Star::Json::PRINTJSON)(playerJson, jsonStarString, 1, false);
	double playerSize = Star::String::length(base + Star::String::LENGTH)(jsonStarString);
	std::string format = "B";
	if (playerSize > 1024) {
		playerSize /= 1024;
		format = "KB";
	}
	if (playerSize > 1024) {
		playerSize /= 1024;
		format = "MB";
	}
	std::string jsonString = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(jsonStarString);
	free(jsonStarString);
	free(playerJson);
	std::ofstream myfile;
	myfile.open("playerDump.json"); myfile << jsonString; myfile.close();
	Star::Lua::createTable(base + Star::Lua::CREATETABLE)(L, 0, 0);
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, "size");
	Star::Lua::pushNumber(base + Star::Lua::PUSHNUMBER)(L, playerSize);
	Star::Lua::setTable(base + Star::Lua::SETTABLE)(L, -3);
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, "format");
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, format.c_str());
	Star::Lua::setTable(base + Star::Lua::SETTABLE)(L, -3);
	return 1;
}

EXTERN_C int  saveSongFile(lua_State* L) {
	int inp = static_cast<int>(lua_tonumber(L, 1));
	std::shared_ptr<void> resultPtr;
	Star::EntityMap::getPlayer(base + Star::EntityMap::GETPLAYER)(entityMapPointer, &resultPtr, inp);
	void* playerPtr = resultPtr.get();
	std::shared_ptr<void> songSharedPtr;
	Star::Player::songbook(playerPtr, &songSharedPtr);
	void* songbookPtr = songSharedPtr.get();
	int songJsonPtrOffset = 0xF0;
	void* songJsonPtr = (reinterpret_cast<void*>(reinterpret_cast<char*>(songbookPtr) + songJsonPtrOffset));
	void* jsonStarString = malloc(sizeof(unsigned char) * 1024 * 1024 * 2);
	Star::Json::printJson(base + Star::Json::PRINTJSON)(songJsonPtr, jsonStarString, 1, false);
	std::string jsonString = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(jsonStarString);
	double songSize = Star::String::length(Star::String::LENGTH)(jsonStarString);
	std::string format = "B";
	if (songSize > 1024) {
		songSize /= 1024;
		format = "KB";
	}
	if (songSize > 1024) {
		songSize /= 1024;
		format = "MB";
	}
	void* abcJson = malloc(1024 * 1024);
	Star::Json::parseJson(base + Star::Json::PARSEJSON)(abcJson, &jsonString);
	std::string key_1 = "resource";
	void* nameStr = malloc(1024 * 1024);
	Star::Json::getString(base + Star::Json::GETSTRING)(abcJson, nameStr, &key_1);
	std::string name = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(nameStr);
	std::ofstream myfile;
	myfile.open("songDump.json"); myfile << jsonString; myfile.close();
	free(abcJson);
	free(jsonStarString);
	free(nameStr);
	Star::Lua::createTable(base + Star::Lua::CREATETABLE)(L, 0, 0);
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, "size");
	Star::Lua::pushNumber(base + Star::Lua::PUSHNUMBER)(L, songSize);
	Star::Lua::setTable(base + Star::Lua::SETTABLE)(L, -3);
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, "format");
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, format.c_str());
	Star::Lua::setTable(base + Star::Lua::SETTABLE)(L, -3);
	return 1;
}

EXTERN_C int getChatFocused(lua_State* L) {
	bool focused = Star::Chat::hasFocus(base + Star::Chat::HASFOCUS)(chatPointer);
	Star::Lua::pushBoolean(base + Star::Lua::PUSHBOOLEAN)(L, focused);
	return 1;
}

EXTERN_C int currentChat(lua_State* L) {
	if (Star::Chat::hasFocus(base + Star::Chat::HASFOCUS)(chatPointer)) {
		void* chatStr = malloc(1024 * 1024);
		Star::Chat::currentChat(base + Star::Chat::CURRENTCHAT)(chatPointer, chatStr);
		std::string res = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(chatStr);
		free(chatStr);
		Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, res.c_str());
	}
	else Star::Lua::pushNil(base + Star::Lua::PUSHNIL)(L);
	return 1;
}

EXTERN_C int  getHexUniqueID(lua_State* L) {
	int inp = static_cast<int>(lua_tonumber(L, 1));
	std::shared_ptr<void> resultPtr;
	Star::EntityMap::getPlayer(base + Star::EntityMap::GETPLAYER)(entityMapPointer, &resultPtr, inp);
	void* playerPtr = resultPtr.get();
	if (playerPtr == nullptr) {
		Star::Lua::pushNil(base + Star::Lua::PUSHNIL)(L);
		return 1;
	}
	void* uuidPtr = malloc(10 * 1024 * 1024);
	Star::Player::uuid(playerPtr, uuidPtr);
	void* uuidString = malloc(sizeof(unsigned char) * 1024 * 1024);
	Star::Uuid::hex(base + Star::Uuid::HEX)(uuidPtr, uuidString);
	std::string res = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(uuidString);
	free(uuidPtr);
	free(uuidString);
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, res.c_str());
	return 1;
}

EXTERN_C int playerDirectives(lua_State* L) {
	int inp = static_cast<int>(lua_tonumber(L, 1));
	std::shared_ptr<void> resultPtr;
	Star::EntityMap::getPlayer(base + Star::EntityMap::GETPLAYER)(entityMapPointer, &resultPtr, inp);
	void* playerPtr = resultPtr.get();
	if (playerPtr == nullptr) {
		Star::Lua::pushNil(base + Star::Lua::PUSHNIL)(L);
		return 1;
	}
	Tech_Controller_Parent_Directives_Hook->SetupHook((BYTE*)(base + Star::TechController::PARENTDIRECTIVES), (BYTE*)&TECH_CONTROLLER_PARENT_DIRECTIVES_HOOK);
	Tech_Controller_Parent_Directives_Hook->Hook();
	tech_Controller_Parent_Directives_hookTramp = Tech_Controller_Parent_Directives_Hook->GetOriginal<Star::TechController::parentDirectives>();
	Status_Controller_Parent_Directives_Hook->SetupHook((BYTE*)(base + Star::StatusController::PARENTDIRECTIVES), (BYTE*)&STATUS_CONTROLLER_PARENT_DIRECTIVES_HOOK);
	Status_Controller_Parent_Directives_Hook->Hook();
	status_Controller_Parent_Directives_hookTramp = Status_Controller_Parent_Directives_Hook->GetOriginal<Star::StatusController::parentDirectives>();
	Star::Lua::createTable(base + Star::Lua::CREATETABLE)(L, 0, 0);
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, "techDirectives");
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, playerTechDirectives.c_str());
	Star::Lua::setTable(base + Star::Lua::SETTABLE)(L, -3);
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, "statusDirectives");
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, playerStatusDirectives.c_str());
	Star::Lua::setTable(base + Star::Lua::SETTABLE)(L, -3);
	Tech_Controller_Parent_Directives_Hook->UnHook();
	Status_Controller_Parent_Directives_Hook->UnHook();
	return 1;
}

EXTERN_C int getPixel(lua_State* L) {
	std::string inpStr = lua_tostring(L, 1);
	unsigned int x = static_cast<unsigned int>(lua_tonumber(L, 2));
	unsigned int y = static_cast<unsigned int>(lua_tonumber(L, 3));
	std::array<unsigned int, 2> pos = { x, y };
	std::shared_ptr<void> imgShared;
	Star::Assets::image(base + Star::Assets::IMAGE)(assetsPointer, &imgShared, &inpStr);
	void* image = imgShared.get();
	std::array<unsigned char, 4> color;
	Star::Image::get(base + Star::Image::GET)(image, &color, &pos);
	int r = (int)color[0];
	int g = (int)color[1];
	int b = (int)color[2];
	int a = (int)color[3];
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(2) << std::hex << r;
	std::string rStr = stream.str();
	stream.str("");
	stream << std::setfill('0') << std::setw(2) << std::hex << g;
	std::string gStr = stream.str();
	stream.str("");
	stream << std::setfill('0') << std::setw(2) << std::hex << b;
	std::string bStr = stream.str();
	stream.str("");
	stream << std::setfill('0') << std::setw(2) << std::hex << a;
	std::string aStr = stream.str();
	std::string hexColor = rStr + gStr + bStr + aStr;
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, hexColor.c_str());
	return 1;
}

EXTERN_C int material(lua_State* L) {
	int x = static_cast<int>(lua_tonumber(L, 1));
	int y = static_cast<int>(lua_tonumber(L, 2));
	std::string layer = lua_tostring(L, 3);
	Star::TileLayer layerNum;
	if (layer == "foreground") {
		layerNum = Star::TileLayer::Foreground;
	}
	else if (layer == "background") {
		layerNum = Star::TileLayer::Background;
	}
	else {
		Star::Logger::log(base + Star::Logger::LOG)(Star::LogLevel::Error, ("Error: " + layer + " is not a valid layer.").c_str());
		return 0;
	}
	std::array<int, 2> position = { x, y };
	unsigned __int64 materialId = Star::WorldClient::material(base + Star::WorldClient::MATERIAL)(worldClientPointer, &position, layerNum);
	Star::Lua::pushInteger(base + Star::Lua::PUSHINTEGER)(L, materialId);
	return 1;
}

EXTERN_C int getPlayerInventory(lua_State* L) {
	int inp = static_cast<int>(lua_tonumber(L, 1));
	void* argsJson = malloc(1024 * 1024 * 5);
	void* argsJsonStr = malloc(1024 * 1024 * 5);
	std::shared_ptr<void> resultPtr;
	Star::EntityMap::getPlayer(base + Star::EntityMap::GETPLAYER)(entityMapPointer, &resultPtr, inp);
	void* playerPtr = resultPtr.get();
	Star::Player::inventory(base + Star::Player::INVENTORY)(playerPtr, &resultPtr);
	void* inventoryPtr = resultPtr.get();
	Star::PlayerInventory::store(base + Star::PlayerInventory::STORE)(inventoryPtr, argsJson);
	Star::Json::printJson(base + Star::Json::PRINTJSON)(argsJson, argsJsonStr, 1, false);
	std::string resultStr = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(argsJsonStr);
	free(argsJson);
	free(argsJsonStr);
	return JsonForTable(L, resultStr);
}

EXTERN_C int  getPlayerFileSize(lua_State* L) {
	int inp = static_cast<int>(lua_tonumber(L, 1));
	void* argsJson = malloc(1024 * 1024);
	std::shared_ptr<void> resultPtr;
	Star::EntityMap::getPlayer(base + Star::EntityMap::GETPLAYER)(entityMapPointer, &resultPtr, inp);
	void* playerPtr = resultPtr.get();
	void* playerJson = (void*)malloc(sizeof(unsigned char) * 1024 * 1024 * 20);
	Star::Player::diskStore(base + Star::Player::DISKSTORE)(playerPtr, playerJson);
	void* jsonStarString = (std::string*)malloc(sizeof(unsigned char) * 1024 * 1024 * 20);
	Star::Json::printJson(base + Star::Json::PRINTJSON)(playerJson, jsonStarString, 1, false);
	free(playerJson);
	double playerSize = Star::String::length(base + Star::String::LENGTH)(jsonStarString);
	std::string format = "B";
	if (playerSize > 1024) {
		playerSize /= 1024;
		format = "KB";
	}
	if (playerSize > 1024) {
		playerSize /= 1024;
		format = "MB";
	}
	std::string resultStr = "{\"size\":" + std::to_string(playerSize) + ",\"format\":\"" + format + "\"}";
	free(jsonStarString);
	return JsonForTable(L, resultStr);
}

EXTERN_C int teamMembers(lua_State* L) {
	std::vector<Star::TeamClient::Member> memberVec;
	Star::TeamClient::members(base + Star::TeamClient::MEMBERS)(teamClientPointer, &memberVec);
	Star::VMCCCUIDS vmc;
	Star::ClientContext::playerWorldId(base + Star::ClientContext::PLAYERWORLDID)(clientContextPointer, &vmc);
	if (memberVec.size() == 0) {
		return 0;
	}
	std::string jsonStr = "{";
	int nameOffset = 0, uuidOffset = 0x20, entityIdOffset = 0x30;
	int healthOffset = 0x34, energyOffset = 0x38, worldOffset = 0x40;
	int positionOffset = 0x88, warpModeOffset = 0x90, portraitOffset = 0x98;
	for (int i = 0; i < memberVec.size(); i++) {
		void* uuidStr = malloc(1024 * 2);
		void* namePtr = reinterpret_cast<void*>(reinterpret_cast<char*>(&memberVec[i]) + nameOffset);
		void* uuidPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(&memberVec[i]) + uuidOffset);
		void* worldPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(&memberVec[i]) + worldOffset);

		int entityID = *reinterpret_cast<int*>(reinterpret_cast<char*>(&memberVec[i]) + entityIdOffset);
		int healthPercentage = *reinterpret_cast<int*>(reinterpret_cast<char*>(&memberVec[i]) + healthOffset);
		int energyPercentage = *reinterpret_cast<int*>(reinterpret_cast<char*>(&memberVec[i]) + energyOffset);

		std::array<float, 2>* postition = reinterpret_cast<std::array<float, 2>*>(reinterpret_cast<char*>(&memberVec[i]) + healthOffset);

		Star::Uuid::hex(base + Star::Uuid::HEX)(uuidPtr, uuidStr);
		std::string uuidString = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(uuidStr);
		std::string nameString = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(namePtr);

		jsonStr += "\"member" + std::to_string(i + 1) + "\" :  {";
		jsonStr += "\"name\" :  \"" + nameString + "\",";
		jsonStr += "\"entityID\" :  " + std::to_string(entityID) + ",";
#if PRIVATE
		jsonStr += "\"uuid\" :  \"" + uuidString + "\",";
#endif // PRIVATE
		jsonStr += "\"healthPercentage\" :  " + std::to_string(healthPercentage) + ",";
		jsonStr += "\"energyPercentage\" :  " + std::to_string(energyPercentage) + ",";
		jsonStr += "\"currentWorld\" :  " + (std::string)(Star::VMCCCUI::vmccuiEquals(base + Star::VMCCCUI::VMCCCUIEQUALS)(&vmc, worldPtr) ? "true," : "false,");
		jsonStr += "\"position\" : " + (std::string)" [" + std::to_string((*postition)[0]) + "," + std::to_string((*postition)[1]) + "]";

		jsonStr += "}";
		free(uuidStr);
		if (i != memberVec.size() - 1)jsonStr += ",";
	}
	jsonStr += "}";
	std::string err;
	json11::Json json = json11::Json::parse(jsonStr, err);

	return JsonForTable(L, jsonStr);
}

EXTERN_C int getWeather(lua_State* L) {
	if (!clientWeatherPointer) {
		Star::Lua::pushNil(base + Star::Lua::PUSHNIL)(L);
		return 1;
	}
	int weartherNameOffset = 0x78 + 0 + 0;
	void* typePtr = reinterpret_cast<void*>(reinterpret_cast<char*>(clientWeatherPointer) + weartherNameOffset);
	std::string weatherType = Star::String::utf8Ptr(base + Star::String::UTF8PTR)(typePtr);
	Star::Lua::pushString(base + Star::Lua::PUSHSTRING)(L, weatherType.c_str());
	return 1;
}

EXTERN_C int setChatMessage(lua_State* L) {
	std::string msg = lua_tostring(L, 1);
	customChatMessage = msg;
	return 0;
}

EXTERN_C int sendChat(lua_State* L) {
	int count = lua_gettop(L);
	std::string msg = lua_tostring(L, 1);
	std::string channel = "World";
	if (count >= 2) channel = lua_tostring(L, 2);
	if (channel == "World")Star::UniverseClient::sendChat(base + Star::UniverseClient::SENDCHAT)(universeClientPointer, &msg, Star_ChatSendMode::World);
	else if (channel == "Local")Star::UniverseClient::sendChat(base + Star::UniverseClient::SENDCHAT)(universeClientPointer, &msg, Star_ChatSendMode::Local);
	else if (channel == "Party")Star::UniverseClient::sendChat(base + Star::UniverseClient::SENDCHAT)(universeClientPointer, &msg, Star_ChatSendMode::Party);
	return 0;
}

EXTERN_C int setTeam(lua_State* L) {
	std::string target = "friendly";
	__int16 teamT = 0;
	int count = lua_gettop(L);
	if (count >= 1)target = lua_tostring(L, 1);
	if (count >= 2)teamT = static_cast<__int16>(lua_tonumber(L, 2));
	if (target != "friendly" && target != "enemy" && target != "passive" && target != "ghostly" && target != "assistant" && target != "environment" && target != "indiscriminate" && target != "pvp" && target != "null") return 0;
	std::string playerDamageTeamStr = "{\"type\":\"" + target + "\",\"team\": " + std::to_string(teamT) + " }";
	Star::DamageTeam dtPlayer;
	void* dtJson = malloc(1024 * 20);
	Star::Json::parseJson(base + Star::Json::PARSEJSON)(dtJson, &playerDamageTeamStr);
	Star::EntityDamageTeam::entityDamageTeam(base + Star::EntityDamageTeam::ENTITYDAMAGETEAM)(&dtPlayer, dtJson);
	Star::Entity::setTeam(base + Star::Entity::SETTEAM)(playerEntityPointer, dtPlayer);
	free(dtJson);
	return 0;
}

EXTERN_C int setInventoryNetworked(lua_State* L) {
	inventoryNetworked = lua_toboolean(L, 1);
	return 0;
}

EXTERN_C int addChatMessage(lua_State* L) {
	std::string msg = lua_tostring(L, 1);
	Star::Player::addChatMessage(base + Star::Player::ADDCHATMESSAGE)(mainPlayerPointer, &msg);
	return 0;
}

EXTERN_C int setignorePickUpItems(lua_State* L) {
	ignorePickUpItems = lua_toboolean(L, 1);
	return 0;
}

EXTERN_C int setTeamName(lua_State* L) {
	teamName = lua_tostring(L, 1);
	return 0;
}

EXTERN_C int setIgnorePhysicsObjectCollisions(lua_State* L) {
	ignorePhysicsObjectCollisions = lua_toboolean(L, 1);
	return 0;
}

EXTERN_C int setIgnoreProjectileCollisions(lua_State* L) {
	ignoreProjectileCollisions = lua_toboolean(L, 1);
	return 0;
}

EXTERN_C int setIgnoreVehicleCollisions(lua_State* L) {
	ignoreVehicleCollisions = lua_toboolean(L, 1);
	return 0;
}

EXTERN_C int setReachEntities(lua_State* L) {
	reachEntities = lua_toboolean(L, 1);
	return 0;
}

EXTERN_C int respawnInWorld(lua_State* L) {
	bool respawn = lua_toboolean(L, 1);
	int respawnInWorldOffset = 0x108;
	bool* respawnInWorldPtr = reinterpret_cast<bool*>(reinterpret_cast<char*>(worldClientPointer) + respawnInWorldOffset);
	*respawnInWorldPtr = respawn;
	return 0;
}

EXTERN_C int test(lua_State* L) {
	return 0;
}

static const struct luaL_Reg luaFuncs[] = {
	{"setName",setName},
	{"setGender",setGender},
	{"setSpecies",setSpecies},
	{"setPersonality",setPersonality},
	{"suicide",suicide},
	{"playerAimPosition",playerAimPosition},
	{"lounge",lounge},
	{"setEmoteState",setEmoteState},
	{"setColor",setColor},
	{"setChatAnimation",setChatAnimation},
	{"setMeunAnimation",setMeunAnimation},
	{"setBodyDirectives",setBodyDirectives},
	{"setEmoteDirectives",setEmoteDirectives},
	{"setFacialHair",setFacialHair},
	{"setFacialMask",setFacialMask},
	{"setHairType",setHairType},
	{"setHairDirectives",setHairDirectives},
	{"forceNude",forceNude},
	{"setTeamPortrait",setTeamPortrait},
	{"invitePlayer",invitePlayer},
	{"setCanTp", setCanTp},
	{"setSpoofUniqueID",setSpoofUniqueID},
	{"getChatFocused",getChatFocused},
	{"currentChat",currentChat},
	{"playerDirectives",playerDirectives},
	{"material",material},
	{"getPlayerInventory",getPlayerInventory},
	{"teamMembers",teamMembers},
	{"getWeather",getWeather},
	{"setChatMessage",setChatMessage},
	{"sendChat",sendChat},
	{"setTeam",setTeam},
	{"setInventoryNetworked",setInventoryNetworked},
	{"addChatMessage",addChatMessage},
	{"setignorePickUpItems",setignorePickUpItems},
	{"getPlayerFileSize",getPlayerFileSize},
	{"setTeamName",setTeamName},
	{"setIgnorePhysicsObjectCollisions",setIgnorePhysicsObjectCollisions},
	{"setIgnoreProjectileCollisions",setIgnoreProjectileCollisions},
	{"setIgnoreVehicleCollisions",setIgnoreVehicleCollisions},
	{"setReachEntities",setReachEntities},
	{"respawnInWorld",respawnInWorld},
#if PRIVATE
	{"joinParty", joinParty},
	{"removeEntity", removeEntity},
	{"removeFromTeam",removeFromTeam},
	{"savePlayerFile",savePlayerFile},
	{"saveSongFile",saveSongFile},
	{"getHexUniqueID",getHexUniqueID},
	{"getPixel",getPixel},
	{"test",test},
#endif // PRIVATE
	{ NULL, NULL }
};

//特殊Hook注入函数
Star::LuaEngine::createContext create_Context_hookTramp;
std::shared_ptr<PLH::Detour> Create_Contextl_Hook(new PLH::Detour);
void* CREATE_CONTEXT_HOOK(void* le, void* result) {
	int lua_StateOffset = 0x10, envRegistryIdOffset = 0x1C, refCounterOffset = 0x8;
	lua_State** LPtr = reinterpret_cast<lua_State**>(reinterpret_cast<char*>(le) + lua_StateOffset);
	lua_State* L = *LPtr;
	int envRegistryId = *reinterpret_cast<int*>(reinterpret_cast<char*>(le) + envRegistryIdOffset);
	Star::Lua::checkStack(base + Star::Lua::CHECKSTACK)(L, 2);
	Star::Lua::rawGeti(base + Star::Lua::RAWGETI)(L, LUA_REGISTRYINDEX, envRegistryId);
	Star::Lua::createTable(base + Star::Lua::CREATETABLE)(L, 0, 0);
	Star::Lua::setFuncs(base + Star::Lua::SETFUNCS)(L, luaFuncs, 0);
	Star::Lua::setField(base + Star::Lua::SETFIELD)(L, -2, "EF");
	Star::Lua::setTop(base + Star::Lua::SETTOP)(L, -2);
	auto tmp = create_Context_hookTramp(le, result);
	return tmp;
}

void hookInit() {
	World_Client_Hook->SetupHook((BYTE*)(base + Star::WorldClient::WORLDCLIENT), (BYTE*)&WORLD_CLIENT_HOOK);
	World_Client_Hook->Hook();
	world_client_hookTramp = World_Client_Hook->GetOriginal<Star::WorldClient::worldClient>();

	Team_Client_Hook->SetupHook((BYTE*)(base + Star::TeamClient::TEAMCLIENT), (BYTE*)&TEAM_CLIENT_HOOK);
	Team_Client_Hook->Hook();
	team_client_hookTramp = Team_Client_Hook->GetOriginal<Star::TeamClient::teamClient>();

	Team_Invoke_Remote_Hook->SetupHook((BYTE*)(base + Star::TeamClient::INVOKEREMOTE), (BYTE*)&TEAM_INVOKE_REMOTE_HOOK);
	Team_Invoke_Remote_Hook->Hook();
	team_invoke_Remote_hookTramp = Team_Invoke_Remote_Hook->GetOriginal<Star::TeamClient::invokeRemote>();

	Entity_Map_Entity_Query_Hook->SetupHook((BYTE*)(base + Star::EntityMap::ENTITYQUERY), (BYTE*)&ENTITY_MAP_ENTITY_QUERY_HOOK);
	Entity_Map_Entity_Query_Hook->Hook();
	entity_Map_Entity_Query_hookTramp = Entity_Map_Entity_Query_Hook->GetOriginal<Star::EntityMap::entityQuery>();

	Entity_Is_Master_Hook->SetupHook((BYTE*)(base + Star::Entity::ISMASTER), (BYTE*)&ENTITY_IS_MASTER_HOOK);
	Entity_Is_Master_Hook->Hook();
	entity_Is_Master_hookTramp = Entity_Is_Master_Hook->GetOriginal<Star::Entity::isMaster>();

	Player_Set_Busy_State_Hook->SetupHook((BYTE*)(base + Star::Player::SETBUSYSTATE), (BYTE*)&PLAYER_SET_BUSY_STATE_HOOK);
	Player_Set_Busy_State_Hook->Hook();
	player_Set_Busy_State_hookTramp = Player_Set_Busy_State_Hook->GetOriginal<Star::Player::setBusyState>();

	Inventory_Pane_Update_Hook->SetupHook((BYTE*)(base + Star::InventoryPane::UPDATE), (BYTE*)&INVENTORY_PANE_UPDATE_HOOK);
	Inventory_Pane_Update_Hook->Hook();
	inventory_Pane_Update_hookTramp = Inventory_Pane_Update_Hook->GetOriginal<Star::InventoryPane::update>();

	Player_Net_Store_Hook->SetupHook((BYTE*)(base + Star::Player::NETSTORE), (BYTE*)&PLAYER_NET_STORE_HOOK);
	Player_Net_Store_Hook->Hook();
	player_Net_Store_hookTramp = Player_Net_Store_Hook->GetOriginal<Star::Player::netStore>();

	Chat_Chat_Hook->SetupHook((BYTE*)(base + Star::Chat::CHAT), (BYTE*)&CHAT_CHAT_HOOK);
	Chat_Chat_Hook->Hook();
	chat_Chat_hookTramp = Chat_Chat_Hook->GetOriginal<Star::Chat::chat>();

	/*Tech_Controller_Parent_Directives_Hook->SetupHook((BYTE*)(base + Star::TechController::PARENTDIRECTIVES), (BYTE*)&TECH_CONTROLLER_PARENT_DIRECTIVES_HOOK);
	Tech_Controller_Parent_Directives_Hook->Hook();
	tech_Controller_Parent_Directives_hookTramp = Tech_Controller_Parent_Directives_Hook->GetOriginal<Star::TechController::parentDirectives>();

	Status_Controller_Parent_Directives_Hook->SetupHook((BYTE*)(base + Star::StatusController::PARENTDIRECTIVES), (BYTE*)&STATUS_CONTROLLER_PARENT_DIRECTIVES_HOOK);
	Status_Controller_Parent_Directives_Hook->Hook();
	status_Controller_Parent_Directives_hookTramp = Status_Controller_Parent_Directives_Hook->GetOriginal<Star::StatusController::parentDirectives>();*/

	Assets_Assets_Hook->SetupHook((BYTE*)(base + Star::Assets::ASSETS), (BYTE*)&ASSETS_ASSETS_HOOK);
	Assets_Assets_Hook->Hook();
	assets_Assets_hookTramp = Assets_Assets_Hook->GetOriginal<Star::Assets::assets>();

	Client_Weather_Client_Weathe_Hook->SetupHook((BYTE*)(base + Star::ClientWeather::CLIENTWEATHER), (BYTE*)&CLIENT_WEATHER_CLIENT_WEATHER_HOOK);
	Client_Weather_Client_Weathe_Hook->Hook();
	client_Weather_Client_Weather_hookTramp = Client_Weather_Client_Weathe_Hook->GetOriginal<Star::ClientWeather::clientWeather>();

	/*Universe_Client_Send_Chat_Hook->SetupHook((BYTE*)(base + Star::UniverseClient::SENDCHAT), (BYTE*)&UNIVERSE_CLIENT_SEND_CHAT_HOOK);
	Universe_Client_Send_Chat_Hook->Hook();
	universe_Client_Send_Chat_hookTramp = Universe_Client_Send_Chat_Hook->GetOriginal<Star::UniverseClient::sendChat>();*/

	Universe_Client_Universe_Client_Hook->SetupHook((BYTE*)(base + Star::UniverseClient::UNIVERSECLIENT), (BYTE*)&UNIVERSE_CLIENT_UNIVERSE_CLIENT_HOOK);
	Universe_Client_Universe_Client_Hook->Hook();
	universe_Client_Universe_Client_hookTramp = Universe_Client_Universe_Client_Hook->GetOriginal<Star::UniverseClient::universeClient>();

	Player_Inventory_Net_Elements_Need_Store_Hook->SetupHook((BYTE*)(base + Star::PlayerInventory::NETELEMENTSNEEDSTORE), (BYTE*)&PLAYER_INVENTORY_NET_ELEMENTS_NEED_STORE_HOOK);
	Player_Inventory_Net_Elements_Need_Store_Hook->Hook();
	player_Inventory_Net_Elements_Need_Store_hookTramp = Player_Inventory_Net_Elements_Need_Store_Hook->GetOriginal<Star::PlayerInventory::netElementsNeedStore>();

	Player_Inventory_Items_Can_Fit_Hook->SetupHook((BYTE*)(base + Star::PlayerInventory::ITEMSCANFIT), (BYTE*)&PLAYER_INVENTORY_ITEMS_CAN_FIT_HOOK);
	Player_Inventory_Items_Can_Fit_Hook->Hook();
	player_Inventory_Items_Can_Fit_hookTramp = Player_Inventory_Items_Can_Fit_Hook->GetOriginal<Star::PlayerInventory::itemsCanFit>();

	Create_Contextl_Hook->SetupHook((BYTE*)(base + Star::LuaEngine::CREATECONTEXT), (BYTE*)&CREATE_CONTEXT_HOOK);
	Create_Contextl_Hook->Hook();
	create_Context_hookTramp = Create_Contextl_Hook->GetOriginal<Star::LuaEngine::createContext>();

	World_Client_Can_Reach_Entity_Hook->SetupHook((BYTE*)(base + Star::WorldClient::CANREACHENTITY), (BYTE*)&WORLD_CLIENT_CAN_REACH_ENTITY_HOOK);
	World_Client_Can_Reach_Entity_Hook->Hook();
	world_Client_Can_Reach_Entity_hookTramp = World_Client_Can_Reach_Entity_Hook->GetOriginal<Star::WorldClient::canReachEntity>();

	Physics_Object_Moving_Collision_Count_Hook->SetupHook((BYTE*)(base + Star::PhysicsObject::MOVINGCOLLISIONCOUNT), (BYTE*)&PHYSICS_OBJECT_MOVING_COLLISION_COUNT_HOOK);
	//Physics_Object_Moving_Collision_Count_Hook->Hook();
	physics_Object_Moving_Collision_Count_hookTramp = Physics_Object_Moving_Collision_Count_Hook->GetOriginal<Star::PhysicsObject::movingCollisionCount>();

	Projectile_Moving_Collision_Count_Hook->SetupHook((BYTE*)(base + Star::Projectile::MOVINGCOLLISIONCOUNT), (BYTE*)&PROJECTILE_MOVING_COLLISION_COUNT_HOOK);
	//Projectile_Moving_Collision_Count_Hook->Hook();
	projectile_Moving_Collision_Count_hookTramp = Projectile_Moving_Collision_Count_Hook->GetOriginal<Star::Projectile::movingCollisionCount>();

	Vehicle_Moving_Collision_Count_Hook->SetupHook((BYTE*)(base + Star::Vehicle::MOVINGCOLLISIONCOUNT), (BYTE*)&VEHICLE_MOVING_COLLISION_COUNT_HOOK);
	//Vehicle_Moving_Collision_Count_Hook->Hook();
	vehicle_Moving_Collision_Count_hookTramp = Vehicle_Moving_Collision_Count_Hook->GetOriginal<Star::Vehicle::movingCollisionCount>();
}

void hookUninit() {
	World_Client_Hook->UnHook();
	Team_Client_Hook->UnHook();
	Team_Invoke_Remote_Hook->UnHook();
	Entity_Map_Entity_Query_Hook->UnHook();
	Player_Set_Busy_State_Hook->UnHook();
	Inventory_Pane_Update_Hook->UnHook();
	Player_Net_Store_Hook->UnHook();
	Chat_Chat_Hook->UnHook();
	Tech_Controller_Parent_Directives_Hook->UnHook();
	/*Tech_Controller_Parent_Directives_Hook->UnHook();
	Status_Controller_Parent_Directives_Hook->UnHook();*/
	Assets_Assets_Hook->UnHook();
	Client_Weather_Client_Weathe_Hook->UnHook();
	//Universe_Client_Send_Chat_Hook->UnHook();
	Universe_Client_Universe_Client_Hook->UnHook();
	Player_Inventory_Net_Elements_Need_Store_Hook->UnHook();
	Player_Inventory_Items_Can_Fit_Hook->UnHook();
	Create_Contextl_Hook->UnHook();
	World_Client_Can_Reach_Entity_Hook->UnHook();
	Physics_Object_Moving_Collision_Count_Hook->UnHook();
	Projectile_Moving_Collision_Count_Hook->UnHook();
	Vehicle_Moving_Collision_Count_Hook->UnHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		hookInit();
		Star::Logger::log(base + Star::Logger::LOG)(Star::LogLevel::Info, "Oracle Load");
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		hookUninit();
	}
	return TRUE;
}

