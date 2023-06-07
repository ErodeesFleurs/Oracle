#include<windows.h>
#include <cstdint>
#include <libloaderapi.h>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <functional>
#include "lua.hpp"

static uintptr_t tbase = 0;

#ifndef _STARLIB_H_ //如果没有引入头文件file.h
#define _STARLIB_H_ //那就引入头文件file.h

enum Star_PlayerMode {
	Casual,
	Survival,
	Hardcore
};
enum Star_ChatSendMode {
	World,//World
	Local,//Local
	Party
};
namespace Star {
	enum TileLayer {
		Foreground,
		Background
	};
	enum TeamType {
		friendly,
		enemy,
		passive,
		ghostly,
		assistant,
		environment,
		indiscriminate,
		pvp
	};
	enum CollisionKind {
		Null,
		None,
		Platform,
		Dynamic,
		Slippery,
		Block
	};
	enum Gender {
		Male,
		Female
	};
	enum Direction {
		Left,
		Right
	};
	enum HumanoidEmote {
		Idle,
		Walk,
		Run,
		Jump,
		Fall,
		Swim,
		SwimIdle,
		Duck,
		Sit,
		Lay
	};
	template<typename T>
	struct MaybeDS {
		T m_data;
		bool m_initialized;
	};
	template<typename T1, __int64 T2>
	struct	BoxDS {
		std::array<T1, T2> m_min;
		std::array<T1, T2> m_max;
	};
	struct  StringDS {
		std::array<char, 20> m_string;
	};
	namespace MessageContext {
		enum Mode {
			Local,
			Party,
			Broadcast,
			Whisper,
			CommandResult,
			RadioMessage,
			World
		};
	}
	namespace Particle {
		enum Type {
			Variance,
			Ember,
			Textured,
			Animated,
			Streak,
			Text
		};
		enum Layer {
			Back,
			Middle,
			Front
		};
	}
	namespace PlatformerAStar {
		enum Action {
			Walk,
			Jump,
			Arc,
			Drop,
			Swim,
			Fly,
			Land
		};
	}
	struct Color {
		std::array<float, 4> m_data;
	};
	namespace Animation {
		enum AnimationMode {
			Stop,
			EndAndDisappear,
			LoopForever
		};
	}
	struct MFSSHSESAPSS {
		std::pair<Star::StringDS, Star::StringDS> value;
		unsigned __int64 hash;
	};
	struct AnimationDS {
		Star::Animation::AnimationMode m_mode;
		Star::StringDS m_directory;
		Star::StringDS m_tbase;
		bool m_appendFrame;
		int m_frameNumber;
		float m_animationCycle;
		float m_animationTime;
		float m_angle;
		std::array<float, 2> m_offset;
		bool m_centered;
		Star::StringDS m_processing;
		Star::Color m_color;
		int m_variantOffset;
		Star::MFSSHSESAPSS m_tagValues;
		int m_frame;
		float m_animationTimer;
		float m_timeToLive;
		bool m_completed;
	};
	struct ParticleDS {
		Star::Particle::Type type;
		float size;
		float tbaseSize;
		Star::StringDS string;
		Star::Color color;
		Star::Color light;
		float fade;
		bool fullbright;
		std::array<float, 2> position;
		std::array<float, 2> velocity;
		std::array<float, 2> finalVelocity;
		std::array<float, 2> approach;
		bool flippable;
		bool flip;
		float rotation;
		float angularVelocity;
		float length;
		BYTE destructionAction[4];
		Star::StringDS destructionImage;
		float destructionTime;
		float timeToLive;
		Star::Particle::Layer layer;
		bool collidesForeground;
		bool collidesLiquid;
		bool underwaterOnly;
		bool ignoreWind;
		bool trail;
		Star::MaybeDS<Star::AnimationDS> animation;
		Star::StringDS directives;
	};
	struct VESLJJ {
		std::_Align_type<double, 16> m_buffer;
		unsigned __int8 m_typeIndex;
	};
	namespace Projectile {
		DWORD64 MOVINGCOLLISIONCOUNT = 0x41C4B0;
		typedef void* (__fastcall* movingCollisionCount)(void* p);
	}
	namespace WeatherType {
	};
	struct  MessageContextDS {
		Star::MessageContext::Mode mode;
		Star::StringDS channelName;
	};
	struct  VMCCCUIDS {
		std::array<char, 0x48>state;
	};
	struct  VMDDDDI {
		std::_Align_type<double, 72> m_buffer;
		unsigned __int8 m_typeIndex;
	};
	struct UuidDS {
		std::array<char, 16> m_data;
	};
	struct ChatReceivedMessageDS {
		Star::MessageContextDS context;
		unsigned __int16 fromConnection;
		Star::StringDS fromNick;
		Star::StringDS portrait;
		Star::StringDS text;
	};
	template<typename T>
	struct WeightedPoolDS {
		std::vector<std::pair<double, T>, std::allocator<std::pair<double, T> > > m_items;
		long double m_totalWeight;
	};
	struct WorldGeometry {
		std::array<unsigned int, 2> m_size;
	};
	namespace Humanoid {
		const DWORD64 SETMOVINGBACKWARDS = 0x244690;
		const DWORD64 SETEMOTESTATE = 0x2443B0;

		typedef void(__fastcall* setMovingBackwardsTemp)(void* h, bool movingBackwards);
		typedef void(__fastcall* setEmoteState)(void* h, Star::HumanoidEmote state);

		auto setMovingBackwards = (Star::Humanoid::setMovingBackwardsTemp)(tbase + Star::Humanoid::SETMOVINGBACKWARDS);
		auto setEmoteStateFunc = (Star::Humanoid::setEmoteState)(tbase + Star::Humanoid::SETEMOTESTATE);
	}

	namespace Assets {
		const DWORD64 ASSETS = 0xE8AE0;
		const DWORD64 ASSETEXISTS = 0xECF10;
		const DWORD64 IMAGE = 0xF1B80;

		typedef std::shared_ptr<void>* (__fastcall* image)(void* as, std::shared_ptr<void>*, void* path);
		typedef void(__fastcall* assets)(void* as, void* settings, void* assetSources);

		auto imageFunc = (Star::Assets::image)(Star::Assets::IMAGE);
		auto assetsFunc = (Star::Assets::assets)(Star::Assets::ASSETS);
	}
	namespace AssetException {
		const DWORD64 ASSETEXCEPTION = 0xE8860;
		typedef void(__fastcall* assetExceptionTemp)(void* ae, std::string message);
	}
	namespace ChatProcessor {
		const DWORD64 CHATPROCESSOR = 0x57C0D0;
		const DWORD64 RENICK = 0x19FDF0;
		const DWORD64 HANDLECOMMAND = 0x19E940;
		typedef void* (__fastcall* renickTemp)(void* cp, void* result, unsigned __int16 clientId, void* nick); //void*(Star::ChatProcessor *this, Star::String *result, unsigned __int16 clientId, Star::String *nick)
		auto renick = (Star::ChatProcessor::renickTemp)(tbase + Star::ChatProcessor::RENICK);
	}
	namespace Chat {
		const DWORD64 CHAT = 0x88D210;
		const DWORD64 CURRENTCHAT = 0x88FB80;
		const DWORD64 HASFOCUS = 0x8907C0;
		const DWORD64 ADDHISTORY = 0x88EFD0;
		const DWORD64 ADDLINE = 0x88F180;
		const DWORD64 ADDMESSAGES = 0x88F350;
		const DWORD64 UPDATE = 0x891BF0;

		typedef void* (__fastcall* currentChat)(void* c, void* result);
		typedef void(__fastcall* addHistoryTemp)(void* c, std::string* ct);
		typedef void(__fastcall* addLineTemp)(void* c, std::string* ct, bool showPane);
		typedef void(__fastcall* addMessagesTemp)(void* c, std::vector<Star::ChatReceivedMessageDS>* msg, bool showPane);
		typedef bool(__fastcall* hasFocus)(void* c);
		typedef bool(__fastcall* chat)(void* c, std::shared_ptr<void> client);

		auto currentChatFunc = (Star::Chat::currentChat)(tbase + Star::Chat::CURRENTCHAT);
		auto addHistory = (Star::Chat::addHistoryTemp)(tbase + Star::Chat::ADDHISTORY);
		auto addLine = (Star::Chat::addLineTemp)(tbase + Star::Chat::ADDLINE);
		auto addMessages = (Star::Chat::addMessagesTemp)(tbase + Star::Chat::ADDMESSAGES);
		auto hasFocusFunc = (Star::Chat::hasFocus)(Star::Chat::HASFOCUS);
		auto chatFunc = (Star::Chat::chat)(Star::Chat::CHAT);
	}
	namespace ChatReceivedMessage {
		const DWORD64 CHATRECEIVEDMESSAGE = 0x1A1DC0;
		typedef void* (__fastcall chatReceivedMessageTemp)(void* crm, void* context, unsigned __int16 fromConnection, void* fromNick, void* text);
	}
	namespace ClientDisconnectRequestPacket {
		const DWORD64 MAKESHARED = 0x2F3AB0;
		typedef std::shared_ptr<void>(__fastcall* make_sharedTemp)(std::shared_ptr<void>* result);
		auto make_shared = (Star::ClientDisconnectRequestPacket::make_sharedTemp)(tbase + Star::ClientDisconnectRequestPacket::MAKESHARED);
	}
	namespace ClientConnectPacket {
		const DWORD64 CLIENTCONNECTPACKET = 0x2F7850;
		typedef void(__fastcall* clientConnectPacketTemp)(void* ccp, void* ad, bool aam, void* uuid, void* name, void* species, void* sc, void* su, bool ic, void* acc);
		auto ClientConnectPacket = (Star::ClientConnectPacket::clientConnectPacketTemp)(tbase + Star::ClientConnectPacket::CLIENTCONNECTPACKET);
	}
	namespace ClientWeather {
		const DWORD64 CLIENTWEATHER = 0x5C5A50;
		const DWORD64 UPDATE = 0x5C90D0;

		typedef void(__fastcall* clientWeather)(void* cw);

		auto clientWeatherFunc = (Star::ClientWeather::clientWeather)(Star::ClientWeather::CLIENTWEATHER);

	}
	namespace ClientApplication {
		const DWORD64 UPDATE = 0x24490;

		typedef void(__fastcall* update)(void* ca);

	}
	namespace CommandProcessor {
		const DWORD64 HELP = 0x1BEF30;
	}
	namespace Image {
		const DWORD64 GET = 0x6A6B0;
		typedef std::array<char, 4>* (__fastcall* get)(void* im, std::array<unsigned char, 4>* result, std::array<unsigned int, 2>* pos);
		auto getFunc = (Star::Image::get)(Star::Image::GET);
	}
	namespace Maybe {
		namespace String {
			const DWORD64 TAKE = 0x95460;
			const DWORD64 MAYBE = 0x18730;
			typedef void* (__fastcall* takeTemp)(void* s, void* result);
			typedef void(__fastcall* maybe)(void* s, void* str);
			auto maybeFunc = (Star::Maybe::String::maybe)(Star::Maybe::String::MAYBE);
		}
	}
	namespace String {
		const DWORD64 STRING = 0xC1490; //void(Star::String *this, std::basic_string<char,std::char_traits<char>,std::allocator<char> > *s)
		const DWORD64 STRINGVOID = 0xC1630;
		const DWORD64 APPEND = 0xC7800;
		const DWORD64 UTF8PTR = 0xCC000;
		const DWORD64 LENGTH = 0xCAB90;
		const DWORD64 CLEAR = 0xC7A80;

		typedef void(__fastcall* string)(void* s, void* in);
		typedef void(__fastcall* stringVoid)(void* s);
		typedef const char* (__fastcall* utf8Ptr)(void* s);
		typedef unsigned __int64(__fastcall* length)(void* s);
		typedef void(__fastcall* append)(void* s, const char* in);
		typedef void(__fastcall* append)(void* s, const char* in);
		typedef void(__fastcall* clear)(void* s);

		auto lengthFunc = (Star::String::length)(tbase + Star::String::LENGTH);
	}
	namespace HumanoidIdentity {
		const DWORD64 TOJSON = 0x244A60;

		typedef void* (__fastcall* toJsonTemp)(void* hi, void* result);
		auto toJson = (Star::HumanoidIdentity::toJsonTemp)(tbase + Star::HumanoidIdentity::TOJSON);
	}
	enum EntityType {
		PlantU,
		ObjectU,
		VehicleU,
		ItemDropU,
		PlantDropU,
		ProjectileU,
		StagehandU,
		MonsterU,
		NpcU,
		PlayerU
	};
	namespace Player {
		const DWORD64 RECEIVEMESSAGE = 0x3C6290;
		const DWORD64 SETSPECIES = 0x3C9000;
		const DWORD64 CURRENCY = 0x3BB1B0;
		const DWORD64 KILL = 0x3C1CE0;
		const DWORD64 SETGENDER = 0x3C8690;
		const DWORD64 SETNAME = 0x3C8A30;
		const DWORD64 SETPERSONALITY = 0x3C8F40;
		const DWORD64 ADDCHATMESSAGE = 0x3B92E0;
		const DWORD64 DISKSTORE = 0x3BB8A0;
		const DWORD64 DRAWABLES = 0x3BC160;
		const DWORD64 SETBODYDIRECTIVES = 0x3C8280;
		const DWORD64 SETEMOTEDIRECTIVES = 0x3C8490;
		const DWORD64 SETHAIRDIRECTIVES = 0x3C8750;
		const DWORD64 SETFACIALHAIR = 0x3C84D0;
		const DWORD64 SETFACIALMASK = 0x3C8540;
		const DWORD64 SETHAIRTYPE = 0x3C8790;
		const DWORD64 SONGBOOK = 0x3C9270;
		const DWORD64 NAME = 0x3C2B40;
		const DWORD64 LOUNGE = 0x3C21B0;
		const DWORD64 UUID = 0x3CD250;
		const DWORD64 AIMPOSITION = 0x3CD250;
		const DWORD64 FORCENUDE = 0x3BEA80;
		const DWORD64 SETNETSTATES = 0x3C8A70;
		const DWORD64 GETNETSTATES = 0x3BF400;
		const DWORD64 INVENTORY = 0x3C19D0;
		const DWORD64 SETADMIN = 0x3C8270;
		const DWORD64 SETBUSYSTATE = 0x3C82C0;
		const DWORD64 WRITENETSTATE = 0x3CD630;
		const DWORD64 NETSTORE = 0x3C2D40;
		const DWORD64 POSITION = 0x3C3640;
		const DWORD64 FORCEREGIONS = 0x3BEAE0;

		typedef void(__fastcall* setSpecies)(void* p, void* species);
		typedef void(__fastcall* kill)(void* p);
		typedef void(__fastcall* setGender)(void* p, bool* gender);
		typedef void(__fastcall* setName)(void* p, void* name);
		typedef void(__fastcall* addChatMessage)(void* p, void* message);
		typedef void(__fastcall* setPersonality)(void* p, void* personality);
		typedef void* (__fastcall* diskStore)(void* p, void* result);
		typedef void* (__fastcall* drawablesTemp)(void* p, void* result);
		typedef void(__fastcall* setBodyDirectives)(void* p, void* directives);
		typedef void(__fastcall* setEmoteDirectives)(void* p, void* directives);
		typedef void(__fastcall* setHairDirectives)(void* p, void* directives);
		typedef void(__fastcall* setFacialHair)(void* p, void* group, void* type, void* directives);
		typedef void(__fastcall* setFacialMask)(void* p, void* group, void* type, void* directives);
		typedef void(__fastcall* setHairTypeTemp)(void* p, std::string* group, std::string* type);
		typedef std::shared_ptr<void>* (__fastcall* songbookTemp)(void* p, std::shared_ptr<void>* result);
		typedef void* (__fastcall* nameTemp)(void* p);
		typedef bool(__fastcall* lounge)(void* p, int loungeableEntityId, unsigned __int64 anchorIndex);
		typedef void* (__fastcall* uuidTemp)(void* p, void* result);
		typedef std::array<float, 2>* (__fastcall* aimPosition)(void* p, std::array<float, 2>* result);
		typedef void(__fastcall* forceNude)(void* p);
		typedef void(__fastcall* setAdminTemp)(void* p, bool isAdmin);
		typedef std::shared_ptr<void>* (__fastcall* inventory)(void* p, std::shared_ptr<void>* res);
		typedef void(__fastcall* setBusyState)(void* p, int index);
		typedef void* (__fastcall* netStore)(void* p, void* res);


		auto setBodyDirectivesFunc = (Star::Player::setBodyDirectives)(Star::Player::SETBODYDIRECTIVES);
		auto setEmoteDirectivesFunc = (Star::Player::setEmoteDirectives)(Star::Player::SETEMOTEDIRECTIVES);
		auto setHairDirectivesFunc = (Star::Player::setHairDirectives)(Star::Player::SETHAIRDIRECTIVES);
		auto setFacialHairFunc = (Star::Player::setFacialHair)(Star::Player::SETFACIALHAIR);
		auto setFacialMaskFunc = (Star::Player::setFacialMask)(Star::Player::SETFACIALMASK);
		auto setHairType = (Star::Player::setHairTypeTemp)(tbase + Star::Player::SETHAIRTYPE);
		auto diskStoreFunc = (Star::Player::diskStore)(Star::Player::DISKSTORE);
		auto killFunc = (Star::Player::kill)(Star::Player::KILL);
		auto songbook = (Star::Player::songbookTemp)(tbase + Star::Player::SONGBOOK);
		auto drawables = (Star::Player::drawablesTemp)(tbase + Star::Player::DRAWABLES);
		auto name = (Star::Player::nameTemp)(tbase + Star::Player::NAME);
		auto loungeFunc = (Star::Player::lounge)(Star::Player::LOUNGE);
		auto aimPositionFunc = (Star::Player::aimPosition)(Star::Player::AIMPOSITION);
		auto forceNudeFunc = (Star::Player::forceNude)(tbase + Star::Player::FORCENUDE);
		auto addChatMessageFunc = (Star::Player::addChatMessage)(Star::Player::ADDCHATMESSAGE);
		auto setAdmin = (Star::Player::setAdminTemp)(tbase + Star::Player::SETADMIN);
		auto uuid = (Star::Player::uuidTemp)(tbase + Star::Player::UUID);
		auto inventoryFunc = (Star::Player::inventory)(Star::Player::INVENTORY);
		auto setBusyStateFunc = (Star::Player::setBusyState)(Star::Player::SETBUSYSTATE);
		auto netStoreFunc = (Star::Player::netStore)(Star::Player::NETSTORE);
	}
	namespace PlayerInventory {
		DWORD64 PLAYERINVENTORY = 0x3E6210;
		DWORD64 MAKESHARED = 0x3AF6D0;
		DWORD64 LOAD = 0x3F1F50;
		DWORD64 STORE = 0x3F68A0;
		DWORD64 NETELEMENTSNEEDSTORE = 0x3F3610;
		DWORD64 ITEMSCANFIT = 0x3F0F30;

		typedef std::shared_ptr<void>* (__fastcall* make_sharedTemp)(std::shared_ptr<void>* result);
		typedef void(__fastcall* loadTemp)(void* pi, void* store);
		typedef void(__fastcall* store)(void* pi, void* result);
		typedef void(__fastcall* netElementsNeedStore)(void* pi);
		typedef unsigned __int64(__fastcall* itemsCanFit)(void* pi, std::shared_ptr<void>* items);

		auto make_shared = (Star::PlayerInventory::make_sharedTemp)(tbase + Star::PlayerInventory::MAKESHARED);
		auto load = (Star::PlayerInventory::loadTemp)(tbase + Star::PlayerInventory::LOAD);
		auto storeFunc = (Star::PlayerInventory::store)(Star::PlayerInventory::STORE);
		auto netElementsNeedStoreFunc = (Star::PlayerInventory::netElementsNeedStore)(Star::PlayerInventory::NETELEMENTSNEEDSTORE);
		auto itemsCanFitFunc = (Star::PlayerInventory::itemsCanFit)(Star::PlayerInventory::ITEMSCANFIT);
	}
	namespace PhysicsObject {
		DWORD64 MOVINGCOLLISIONCOUNT = 0x6EED20;
		typedef unsigned __int64(__fastcall* movingCollisionCount)(void* po);
	}
	namespace Vehicle {
		DWORD64 MOVINGCOLLISIONCOUNT = 0x5B6130;
		typedef void* (__fastcall* movingCollisionCount)(void* v);
	}
	namespace InventoryPane {
		DWORD64 UPDATE = 0x8D56F0;

		typedef void(__fastcall* update)(void* ip);
		auto updateFunc = (Star::InventoryPane::update)(Star::InventoryPane::UPDATE);
	}
	namespace IOException {
		const DWORD64 IOEXCEPTION = 0x68AF0;
		typedef void(__fastcall* ioExceptionTemp)(std::string message, void* ec);
	}
	namespace StarException {
		const DWORD64 STAREXCEPTION = 0xD9020;
		typedef void(__fastcall* starExceptionTemp)(void* se, const char* type, std::string message, const std::exception* cause);
	}
	namespace Monster {
		const DWORD64 DESTROY = 0x2B9B40;

		typedef void(__fastcall* destroy)(void* m, void* renderCallback);
	}
	namespace SongBook {
		const DWORD64 PLAY = 0x49D0D0;

		typedef void(__fastcall* playTemp)(void* sb, void* song, void* timeSource);
		auto play = (Star::SongBook::playTemp)(tbase + Star::SongBook::PLAY);
	}
	enum LogLevel {
		Debug,
		Info,
		Warn,
		Error
	};
	namespace Logger {
		const DWORD64 LOG = 0x9CBA0;
		const DWORD64 LOGF = 0x156E0;

		typedef void(__fastcall* log)(Star::LogLevel level, const char* msg);
	}
	namespace WorldClient {
		const DWORD64 WORLDCLIENT = 0x5DDDA0;
		const DWORD64 OVERRIDEGRAVITY = 0x5F4780;
		const DWORD64 REMOVEENTITY = 0x5F6BB0;
		const DWORD64 CLEARWORLD = 0x5E9F10;
		const DWORD64 TRYGIVEMAINPLAYERITEM = 0x5FBB50;
		const DWORD64 MATERIAL = 0x5F3C90;
		const DWORD64 UPDATE = 0x5FBD80;
		const DWORD64 FORCERELIONS = 0x5EE530;
		const DWORD64 SETUPFORCERELIONS = 0x5FA410;
		const DWORD64 GETPROPERTY = 0x5EF1E0;
		const DWORD64 SETPROPERTY = 0x5FA250;
		const DWORD64 CANREACHENTITY = 0x5E8980;

		typedef void(__fastcall* worldClient)(void* wc, std::shared_ptr<void> mainPlayer);
		typedef void(__fastcall* overrideGravity)(void* wc, float gravity);
		typedef void(__fastcall* removeEntity)(void* wc, int entityId, bool andDie);
		typedef void(__fastcall* clearWorld)(void* wc);
		typedef unsigned __int64(__fastcall* material)(void* wc, std::array<int, 2>* pos, Star::TileLayer layer);
		typedef bool(__fastcall* canReachEntity)(void* wc, std::array<float, 2>* position, float radius, int targetEntity, bool preferInteractive);

		auto worldClientFunc = (Star::WorldClient::worldClient)(Star::WorldClient::WORLDCLIENT);
		auto removeEntityFunc = (Star::WorldClient::removeEntity)(Star::WorldClient::REMOVEENTITY);
		auto materialFunc = (Star::WorldClient::material)(tbase + Star::WorldClient::MATERIAL);
	}
	namespace WorldServer {
		const DWORD64 WORLDSERVER = 0x64FC30;
		const DWORD64 REMOVEENTITY = 0x66E750;
		const DWORD64 THREATLEVEL = 0x673A40;
		const DWORD64 PLAYERS = 0x66ABD0;

		typedef void(__fastcall* removeEntity)(void* ws, int entityId, bool andDie);
		typedef float(__fastcall* threatLevel)(void* ws);
		typedef std::vector<int, std::allocator<int> >* (__fastcall* playersTemp)(void* ws, std::vector<int, std::allocator<int> >* res);

		auto players = (Star::WorldServer::playersTemp)(tbase + Star::WorldServer::PLAYERS);
	}
	namespace WorldImpl {
		const DWORD64 CANREACHENTITY = 0x5D3D20;
		typedef bool(__fastcall* canReachEntity)(void* geometry, void* tileSectorArray, void* entityMap, std::array<float, 2>* sourcePosition, float maxRange, int targetEntity, bool preferInteractive);
	}
	namespace VMCCCUI {
		const DWORD64 VMCCCUIEQUALS = 0x433430;
		typedef bool(__fastcall* vmccuiEquals)(void* vm1, void* vm2);

		auto vmccuiEqualsFunc = (Star::VMCCCUI::vmccuiEquals)(Star::VMCCCUI::VMCCCUIEQUALS);
	}
	namespace TeamClient {
		struct Member {
			std::array<char, 0xB0>state;//占位
		};

		const DWORD64 TEAMCLIENT = 0x51F9A0;
		const DWORD64 ACCEPTINVITATION = 0x520FD0;
		const DWORD64 INVOKEREMOTE = 0x521DD0;
		const DWORD64 ISTEAMLEADER = 0x521F50;
		const DWORD64 STATUSUPDATE = 0x522B00;
		const DWORD64 INVITEPLAYER = 0x5219F0;
		const DWORD64 REMOVEFROMTEAM = 0x5227E0;
		const DWORD64 MEMBERS = 0x522360;

		typedef void(__fastcall* teamClient)(void* tc, std::shared_ptr<void> mainPlayer, std::shared_ptr<void> clientContext);
		typedef void(__fastcall* acceptInvitation)(void* rc, void* inviterUuid);
		typedef void(__fastcall* invokeRemote)(void* tc, void* method, void* args, std::function<void __cdecl(void* const&)> responseFunction);
		typedef void(__fastcall* invitePlayerTemp)(void* tc, void* playerName);
		typedef void(__fastcall* removeFromTeam)(void* tc, void* uuid);
		typedef void* (__fastcall* members)(void* tc, void* res);

		auto teamClientFunc = (Star::TeamClient::teamClient)(Star::TeamClient::TEAMCLIENT);
		auto invokeRemoteFunc = (Star::TeamClient::invokeRemote)(Star::TeamClient::INVOKEREMOTE);
		auto invitePlayer = (Star::TeamClient::invitePlayerTemp)(tbase + Star::TeamClient::INVITEPLAYER);
		auto removeFromTeamFunc = (Star::TeamClient::removeFromTeam)(Star::TeamClient::REMOVEFROMTEAM);
		auto membersFunc = (Star::TeamClient::members)(Star::TeamClient::MEMBERS);


	}
	namespace TeamBar {
		const DWORD64 TEAMBAR = 0x917D70;
		const DWORD64 ACCEPTINVITATION = 0x919530;
		const DWORD64 INVITEPLAYER = 0x91ACE0;

		typedef void(__fastcall* acceptInvitationTemp)(void* tc, void* inviterUuid);
		typedef void(__fastcall* invitePlayerTemp)(void* tc, void* playerName);

		auto invitePlayer = (Star::TeamBar::invitePlayerTemp)(tbase + Star::TeamBar::INVITEPLAYER);
	}
	namespace TeamMemberMenu {
		const DWORD64 UPDATEWIDGETS = 0x91BF50;
	}
	namespace Uuid {
		const DWORD64 UUID = 0xD05C0; // Star::Uuid::Uuid(Star::Uuid *this, Star::String *hex)
		const DWORD64 HEX = 0xD0860;
		const DWORD64 UUIDBA = 0xD04F0;
		const DWORD64 OPEQUAL = 0xD0700;

		typedef void* (__fastcall* uuid)(void* u, void* hex); // Star::Uuid::Uuid(Star::Uuid *this, Star::String *hex)
		typedef void* (__fastcall* hex)(void* u, void* result);
		typedef bool(__fastcall* opequal)(void* u, void* com);

		auto uuidFunc = (Star::Uuid::uuid)(Star::Uuid::UUID);
	}
	namespace Json {
		const DWORD64 PRINTJSON = 0x7F0C0;
		const DWORD64 SIZE = 0x81E30;
		const DWORD64 PARSEJSON = 0x7EAE0;
		const DWORD64 GETFROMKEY = 0x7C0A0; //Star::Json * Star::Json::get(Star::Json *this, Star::Json *result, Star::String *key)
		const DWORD64 TYPENAME = 0x82640;
		const DWORD64 SETKEY = 0x81890; //Star::Json * Star::Json::set(Star::Json *this, Star::Json *result, Star::String key, Star::Json value)
		const DWORD64 JSONFROMSTRING = 0x77820; // Star::Json::Json(Star::Json *this, Star::String s)

		const DWORD64 JSONBOOL = 0x779B0;
		const DWORD64 JSONDOUBLE = 0x773D0;
		const DWORD64 JSONINT = 0x77330;
		const DWORD64 JSONSTRING = 0x77750;

		const DWORD64 CONTAINS = 0x7B840;

		const DWORD64 TOBOOL = 0x82240;
		const DWORD64 TODOUBLE = 0x822A0;
		const DWORD64 TOINT = 0x82330;
		const DWORD64 TOSTRING = 0x82440;

		const DWORD64 GETBOOL = 0x7C500;
		const DWORD64 GETDOUBLE = 0x7C6F0;
		const DWORD64 GETINT = 0x7C910;
		const DWORD64 GETSTRING = 0x7CBE0;

		const DWORD64 INSERT = 0x7CBE0;

		typedef std::string* (__fastcall* printJson)(void* j, void* result, int pretty, bool sort);
		typedef unsigned __int64(__fastcall* size)(void* j);
		typedef void* (__fastcall* parseJson)(void* result, void* json);
		typedef void* (__fastcall* getFromKeyTempy)(void* j, void* result, std::string* key);
		typedef void(__fastcall* typeNameTemp)(void* j, void* result);
		typedef void* (__fastcall* setKey)(void* j, void* result, void* key, void* value);
		typedef void(__fastcall* JsonFromString)(void* j, void* s);

		typedef void(__fastcall* JsonBoolTemp)(void* j, bool b);
		typedef void(__fastcall* JsonDoubleTemp)(void* j, long double ld);
		typedef void(__fastcall* JsonIntTemp)(void* j, int i);
		typedef void(__fastcall* JsonStringTemp)(void* j, std::string* s);

		typedef bool(__fastcall* containsTemp)(void* j, std::string* s);

		typedef bool(__fastcall* ToBoolTemp)(void* j);
		typedef double(__fastcall* ToDoubleTemp)(void* j);
		typedef __int64(__fastcall* ToIntTemp)(void* j);
		typedef void* (__fastcall* ToStringTemp)(void* j, std::string* result);

		typedef bool(__fastcall* getBoolTemp)(void* j, std::string* key);
		typedef double(__fastcall* getDoubleTemp)(void* j, std::string* key);
		typedef __int64(__fastcall* getIntTemp)(void* j, std::string* key);
		typedef void* (__fastcall* getString)(void* j, void* s, void* key);

		typedef void* (__fastcall* insertTemp)(void* j, void* res, unsigned __int64 index, void* v);


		auto printJsonFunc = (Star::Json::printJson)(tbase + Star::Json::PRINTJSON);
		auto typeName = (Star::Json::typeNameTemp)(tbase + Star::Json::TYPENAME);
		auto getFromKey = (Star::Json::getFromKeyTempy)(tbase + Star::Json::GETFROMKEY);

		auto JsonBool = (Star::Json::JsonBoolTemp)(tbase + Star::Json::JSONBOOL);
		auto JsonDouble = (Star::Json::JsonDoubleTemp)(tbase + Star::Json::JSONDOUBLE);
		auto JsonInt = (Star::Json::JsonIntTemp)(tbase + Star::Json::JSONINT);
		auto JsonString = (Star::Json::JsonStringTemp)(tbase + Star::Json::JSONSTRING);

		auto contains = (Star::Json::containsTemp)(tbase + Star::Json::CONTAINS);

		auto ToBool = (Star::Json::ToBoolTemp)(tbase + Star::Json::TOBOOL);
		auto ToDouble = (Star::Json::ToDoubleTemp)(tbase + Star::Json::TODOUBLE);
		auto ToInt = (Star::Json::ToIntTemp)(tbase + Star::Json::TOINT);
		auto ToString = (Star::Json::ToStringTemp)(tbase + Star::Json::TOSTRING);

		auto getBool = (Star::Json::getBoolTemp)(tbase + Star::Json::GETBOOL);
		auto getDouble = (Star::Json::getDoubleTemp)(tbase + Star::Json::GETDOUBLE);
		auto getInt = (Star::Json::getIntTemp)(tbase + Star::Json::GETINT);
		auto getStringFunc = (Star::Json::getString)(Star::Json::GETSTRING);

		auto insert = (Star::Json::insertTemp)(tbase + Star::Json::INSERT);
	}
	struct  DamageTeam {
		Star::CollisionKind type;
		unsigned __int16 team;
	};
	namespace EntityDamageTeam {
		const DWORD64 ENTITYDAMAGETEAM = 0x1D69E0;
		const DWORD64 TOJSON = 0x1D8A10;
		typedef void* (__fastcall* entityDamageTeam)(Star::DamageTeam* dt, void* json);
		typedef void* (__fastcall* toJson)(Star::DamageTeam* dt, void* result);
		auto EntityDamageTeamFunc = (Star::EntityDamageTeam::entityDamageTeam)(tbase + Star::EntityDamageTeam::ENTITYDAMAGETEAM);
		auto toJsonFunc = (Star::EntityDamageTeam::toJson)(Star::EntityDamageTeam::TOJSON);
	}
	namespace EntityMap {
		const DWORD64 ENTITYMAP = 0x21AC70;
		const DWORD64 REMOVEENTITY = 0x222660;
		const DWORD64 ENTITY = 0x220220;
		const DWORD64 GETPLAYER = 0x5D7D50;
		const DWORD64 ENTITYQUERY = 0x220280;

		typedef std::shared_ptr<void>* (__fastcall* removeEntity)(void* em, std::shared_ptr<void>* result, int entityId);
		typedef std::shared_ptr<void>* (__fastcall* entityTemp)(void* em, std::shared_ptr<void>* result, int entityId);
		typedef std::shared_ptr<void>* (__fastcall* getPlayer)(void* em, std::shared_ptr<void>* result, int entityId);
		typedef void* (__fastcall* entityQuery)(void* em, void* result, void* boundBox, void* filter);

		auto removeEntityFunc = (Star::EntityMap::removeEntity)(Star::EntityMap::REMOVEENTITY);
		auto entity = (Star::EntityMap::entityTemp)(tbase + Star::EntityMap::ENTITY);
		auto getPlayerFunc = (Star::EntityMap::getPlayer)(Star::EntityMap::GETPLAYER);
		auto entityQueryFunc = (Star::EntityMap::entityQuery)(Star::EntityMap::ENTITYQUERY);
	}
	namespace Entity {
		const DWORD64 UNIQUEID = 0x6A2390;
		const DWORD64 SETUNIQUEID = 0x6A2310;
		const DWORD64 ISMASTER = 0x6A2210;
		const DWORD64 GETTEAM = 0x6A11B0;
		const DWORD64 SETTEAM = 0x6A2300;
		const DWORD64 WRITENETSTATE = 0x6A24A0;

		typedef void* (__fastcall* uniqueId)(void* e, void* result);
		typedef void(__fastcall* setUniqueId)(void* e, void* s);
		typedef void* (__fastcall* getTeamTemp)(void* e, void* result);
		typedef void* (__fastcall* setTeam)(void* e, Star::DamageTeam);
		typedef bool(__fastcall* isMaster)(void* e);

		auto uniqueIdFunc = (Star::Entity::uniqueId)(Star::Entity::UNIQUEID);
		auto setUniqueIdFunc = (Star::Entity::setUniqueId)(Star::Entity::SETUNIQUEID);
		auto getTeam = (Star::Entity::getTeamTemp)(tbase + Star::Entity::GETTEAM);
		auto setTeamFunc = (Star::Entity::setTeam)(tbase + Star::Entity::SETTEAM);
		auto isMasterFunc = (Star::Entity::isMaster)(Star::Entity::SETTEAM);
	}
	namespace EntityCreatePacket {
		const DWORD64  ENTITYCREATEPACKET = 0x2F7DA0;
	}
	namespace SpawnEntityPacket {
		const DWORD64 SPAWNENTITYPACKET = 0x2F87E0;
	}
	namespace MovementController {
		const DWORD64 SETPOSITION = 0x2E8BF0;
		const DWORD64 POSITION = 0x2E7960;
		const DWORD64 UPDATEFORCEREGIONS = 0x2EBB40;

		typedef void(__fastcall* setPositionTemp)(void* mc, std::array<float, 2>arr);
		typedef std::array<float, 2>* (__fastcall* positionTemp)(void* mc, std::array<float, 2>res);
		auto setPosition = (Star::MovementController::setPositionTemp)(tbase + Star::MovementController::SETPOSITION);
		auto position = (Star::MovementController::positionTemp)(tbase + Star::MovementController::POSITION);
	}
	namespace Lua {
		const DWORD64 LUA_NEWSTATE = 0x42180;
		const DWORD64 S_INIT = 0x427B0;
		const DWORD64 NEXT = 0x29260;
		const DWORD64 GETTOP = 0x28C10;
		const DWORD64 CREATETABLE = 0x285B0;
		const DWORD64 CHECKSTACK = 0x28220;
		const DWORD64 RAWSETI = 0x29CD0;
		const DWORD64 RAWGETI = 0x299A0;
		const DWORD64 RAWSET = 0x29BF0;
		const DWORD64 SETTOP = 0x29600;
		const DWORD64 SETTABLE = 0x2A310;
		const DWORD64 GETTABLE = 0x28B60;
		const DWORD64 SETGLOBAL = 0x2A0A0;
		const DWORD64 GETGLOBAL = 0x28940;
		const DWORD64 SETFIELD = 0x29FC0;
		const DWORD64 GETFIELD = 0x28860;
		const DWORD64 PUSHSTRING = 0x29620;
		const DWORD64 PUSHNUMBER = 0x29600;
		const DWORD64 PUSHCCLOSURE = 0x29450;
		const DWORD64 PUSHNIL = 0x295E0;
		const DWORD64 PUSHBOOLEAN = 0x29430;
		const DWORD64 PUSHINTEGER = 0x29540;
		const DWORD64 SETFUNCS = 0x2C9D0;

		typedef int(__fastcall* next)(lua_State* L, int idx);
		typedef int(__fastcall* getTop)(lua_State* L);
		typedef int(__fastcall* createTable)(lua_State* L, int narray, int nrec);
		typedef int(__fastcall* checkStack)(lua_State* L, int n);
		typedef int(__fastcall* rawSeti)(lua_State* L, int idx, __int64 n);
		typedef int(__fastcall* rawGeti)(lua_State* L, int idx, __int64 n);
		typedef void(__fastcall* rawSet)(lua_State* L, int idx);
		typedef void(__fastcall* setTop)(lua_State* L, int idx);
		typedef void(__fastcall* setTable)(lua_State* L, int idx);
		typedef __int64(__fastcall* getTable)(lua_State* L, int idx);
		typedef void(__fastcall* setGlobal)(lua_State* L, const char* name);
		typedef __int64(__fastcall* getGlobal)(lua_State* L, const char* name);
		typedef __int64(__fastcall* setField)(lua_State* L, int idx, const char* k);
		typedef __int64(__fastcall* getField)(lua_State* L, int idx, const char* k);
		typedef char* (__fastcall* pushString)(lua_State* L, const char* s);
		typedef void(__fastcall* pushNumber)(lua_State* L, long double n);
		typedef void(__fastcall* pushCclosure)(lua_State* L, int(__fastcall* fn)(lua_State*), int n);
		typedef void(__fastcall* pushNil)(lua_State* L);
		typedef void(__fastcall* pushBoolean)(lua_State* L, bool b);
		typedef void(__fastcall* pushInteger)(lua_State* L, __int64 n);
		typedef void(__fastcall* setFuncs)(lua_State* L, const luaL_Reg* l, unsigned int nup);

		auto rawGetiFunc = (Star::Lua::rawGeti)(Star::Lua::RAWGETI);
	}
	namespace LuaEngine {
		const DWORD64 CREATECONTEXT = 0xAB310;
		const DWORD64 TABLESET = 0xB05D0;
		const DWORD64 SETGLOAB = 0xA2700;

		typedef void(__fastcall* tableSetTemp)(void* le, bool raw, int handleIndex, const char* key, int(*)(lua_State*));
		typedef void(__fastcall* setGloabTemp)(void* le, int handleIndex, const char* key, int(*)(lua_State*));
		typedef void* (__fastcall* createContext)(void* le, void* res);

		auto createContextFunc = (Star::LuaEngine::createContext)(Star::LuaEngine::CREATECONTEXT);
	}
	namespace TechController {
		const DWORD64 PARENTDIRECTIVES = 0x535340;

		typedef void* (__fastcall* parentDirectives)(void* tc, void* result);
		auto parentDirectivesFunc = (Star::TechController::parentDirectives)(Star::TechController::PARENTDIRECTIVES);
	}
	namespace StatusController {
		const DWORD64 PARENTDIRECTIVES = 0x4E3900;
		const DWORD64 NETSTORE = 0x4E3550;

		typedef void* (__fastcall* parentDirectives)(void* tc, void* result);
		auto parentDirectivesFunc = (Star::StatusController::parentDirectives)(Star::StatusController::PARENTDIRECTIVES);
	}
	namespace MainInterface {
		const DWORD64 MAININTERFACE = 0x8E7210;
		const DWORD64 DOCHAT = 0x8ECF20;

		typedef void(__fastcall* doChat)(void* mi, void* chat, bool addToHistory);
	}
	namespace UniverseClient {
		const DWORD64 UNIVERSECLIENT = 0x5626D0;
		const DWORD64 SENDCHAT = 0x565DB0;
		const DWORD64 SETMAINPLAYER = 0x565F00;
		const DWORD64 PULLCHATMESSAGES = 0x565B40;

		typedef void(__fastcall* universeClient)(void* uc, std::shared_ptr<void> playerStorage, std::shared_ptr<void> statistics);
		typedef void(__fastcall* sendChat)(void* uc, void* text, Star_ChatSendMode sendMode);
		typedef void(__fastcall* setMainplayerTemp)(void* uc, std::shared_ptr<void> player);

		auto universeClientFunc = (Star::UniverseClient::universeClient)(Star::UniverseClient::UNIVERSECLIENT);
		auto sendChatFunc = (Star::UniverseClient::sendChat)(Star::UniverseClient::SENDCHAT);
		auto setMainPlayer = (Star::UniverseClient::setMainplayerTemp)(tbase + Star::UniverseClient::SETMAINPLAYER);
	}
	namespace UniverseConnection {
		const DWORD64 UNIVERSECONNECTION = 0x568610;
		const DWORD64 ISOPEN = 0x56A320;
		const DWORD64 PUSHSINGLE = 0x56AA50;

		typedef void(__fastcall* pushSingleTemp)(void* ucn, std::shared_ptr<void> packet);
		auto pushSingle = (Star::UniverseConnection::pushSingleTemp)(tbase + Star::UniverseConnection::PUSHSINGLE);
	}
	namespace InterfaceCursor {
		const DWORD64 DRAWABLE = 0x8D8000;
	}
	namespace Drawable {
		const DWORD64 SCALE = 0x1DD9E0;
		typedef void(__fastcall* scale)(void* d, float scaling, std::array<float, 2>* scaleCenter);
	}
	namespace SayChatAction {
		const DWORD64 SAYCHATACTION = 0x199F60;
	}
	namespace PcP2PNetworkingService {
		const DWORD64 SETACTIVITYDATA = 0x7F7C50;
	}
	namespace NetworkedAnimator {
		const DWORD64 NETWORKEDANIMATOR = 0x30F220;

		typedef void(__fastcall* NetworkedAnimatorTemp)(void* na, void* config, void* relativePath);

		auto NetworkedAnimator = (Star::NetworkedAnimator::NetworkedAnimatorTemp)(tbase + Star::NetworkedAnimator::NETWORKEDANIMATOR);
	}
	namespace ClientContext {
		const DWORD64 CLIENTCONTEXT = 0x1A5BD0;
		const DWORD64 PLAYERWORLDID = 0x1A9B90;

		typedef void* (__fastcall* playerWorldId)(void* cc, void* res);
		auto playerWorldIdFunc = (Star::ClientContext::playerWorldId)(tbase + Star::ClientContext::PLAYERWORLDID);
	}
	namespace ServerClientContext {
		const DWORD64 SERVERCLIENTCONTEXT = 0x47C5F0;
		const DWORD64 SETTEAM = 0x47EC50;
		const DWORD64 PLAYERUUID = 0x47E190;
		const DWORD64 PLAYERNAME = 0x68C610;
		const DWORD64 PLAYERWORLD = 0x47E220;

		typedef void(__fastcall* SetTeam)(void* scc, Star::DamageTeam dt);
		typedef void* (__fastcall* PlayerUUIDTemp)(void* scc);
		typedef void* (__fastcall* PlayerNameTemp)(void* scc);
		typedef Star::VMCCCUIDS* (__fastcall* PlayerWorldTemp)(void* scc, Star::VMCCCUIDS* res);

		auto setTeamFunc = (Star::ServerClientContext::SetTeam)(Star::ServerClientContext::SETTEAM);
		auto playerUUID = (Star::ServerClientContext::PlayerUUIDTemp)(tbase + Star::ServerClientContext::PLAYERUUID);
		auto playerName = (Star::ServerClientContext::PlayerNameTemp)(tbase + Star::ServerClientContext::PLAYERNAME);
		auto playerWorld = (Star::ServerClientContext::PlayerWorldTemp)(tbase + Star::ServerClientContext::PLAYERWORLD);
	}
	namespace ActorMovementController {
		const DWORD64 POSITION = 0x1212D0;
	}
	namespace NetElementFloating {
		const DWORD64 SET = 0x2E8950;
	}

	const DWORD64 UTF16TOSTRING = 0xD5C00;
	typedef void* (__fastcall* utf16ToString)(void* result, const wchar_t* s);

	const DWORD64 PARSEPERSONALITY = 0x23F430;
	typedef void* (__fastcall* parsePersonality)(void* result, void* config);

	const DWORD64 ENTITYASMONSTER = 0x212430;
	typedef std::shared_ptr<void>* (__fastcall* entityAsMonster)(void* result, void* p);
	struct ByteArray {
		char* m_data;
		unsigned __int64 m_capacity;
		unsigned __int64 m_size;
	};
	namespace NetElementBasicField {
		const DWORD64 PUSHJSON = 0x2C1F80;
	}
}

namespace discord {
	namespace PartySize {
		const DWORD64 SETMAXSIZE = 0x80E820;
	}
}

#endif
