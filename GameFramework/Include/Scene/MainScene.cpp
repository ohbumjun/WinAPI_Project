// Gamae
#include "../GameManager.h"

// Scene
#include "MainScene.h"
#include "SceneResource.h"
#include "SceneManager.h"
#include "HomeScene.h"
// Object
#include "../MonsterInfo.h"
#include "../Object/Player.h"
#include "../Object/Bullet.h"
#include "../Object/Monster.h"
#include "../Object/DuckMonster.h"
#include "../Object/WallObject.h"
#include "../Object/EffectHit.h"
#include "../Object/EffectDash.h"
#include "../Object/TeleportMouse.h"
#include "../Object/StageDoor.h"
#include "../Object/Potion.h"
#include "../Object/HPPotion.h"
#include "../Object/MPPotion.h"
// UI
#include "Camera.h"
#include "../UI/UIMain.h"
#include "../UI/UICharacterStateHUD.h"
#include "../UI/UIImage.h"
// Map
#include "../Map/ScrollMap.h"
#include "../Map/TileMap.h"

CMainScene::CMainScene()
{
}

CMainScene::~CMainScene()
{
}

bool CMainScene::Init()
{
	LoadSound();
	LoadAnimationSequence();
	SetBasicProtoTypes();
	SetBasicUIs();
	
	// Player
	CPlayer *Player = CreatePlayer("Player", Vector2(230.f, 530.f));
	SetPlayer(Player);

	// Camera
	GetCamera()->SetTarget(Player);
	GetCamera()->SetTargetPivot(0.5f, 0.5f);

	// Gun
	CGun* PistolLightGun = CreateObject<CGun>(GUN_PISTOL_LIGHT, GUN_PISTOL_LIGHT_PROTO);
	
	/*
	CDuckMonster*DuckMonster = CreateObject<CDuckMonster>("Monster1",MONSTER_DUCK1_PROTO,
		Vector2(300.f , 530.f));
	DuckMonster->Equip(PistolLightGun);
	DuckMonster->SetCharacterInfo(NORMAL_MONSTER_ATTACK, NORMAL_MONSTER_ARMOR, NORMAL_MONSTER_HP_MAX,
		NORMAL_MONSTER_MP_MAX, 1, 100, 100, 100, NORMAL_MONSTER_ATTACK_DISTANCE, NORMAL_MONSTER_DASH_DISTANCE);
	
	DuckMonster->SetMoveSpeed(NORMAL_MONSTER_MOVE_SPEED);
	*/

	/*
	PistolLightGun = CreateObject<CGun>(GUN_PISTOL_LIGHT, GUN_PISTOL_LIGHT_PROTO);
	DuckMonster = CreateObject<CDuckMonster>("Monster2", MONSTER_DUCK2_PROTO, 
	Vector2(400.f , 700.f));
	DuckMonster->Equip(PistolLightGun);

	PistolLightGun = CreateObject<CGun>(GUN_PISTOL_LIGHT, GUN_PISTOL_LIGHT_PROTO);
	DuckMonster = CreateObject<CDuckMonster>("Monster3", MONSTER_DUCK3_PROTO, 
		Vector2(300.f, 1200.f + (float)(rand() % 1000)));
	DuckMonster->Equip(PistolLightGun);
	*/

	// Stage Door
	CStageDoor *StageDoor_One = CreateObject<CStageDoor>("StageDoor1",
														 Vector2(900.f + rand() % 700, 300.f + rand() % 100),
														 Vector2(50.f, 50.f));
	StageDoor_One->SetDoorStageType(EDoorStage_Type::Stage_Home);

	StageDoor_One = CreateObject<CStageDoor>("StageDoor2",
		Vector2(1500.f, 400.f + rand() % 100),
		Vector2(50.f, 50.f));
	StageDoor_One->SetDoorStageType(EDoorStage_Type::Stage_Two);

	// Windows
	CUIMain *MainWindow = CreateUIWindow<CUIMain>("MainWindow");

	// Potion
	CMPPotion *MPPotion1 = CreateObject<CMPPotion>(POTION_MP_PROTO,POTION_MP_PROTO);
	MPPotion1->SetPos(Vector2(300.f, 230.f));

	CHPPotion *HPPotion1 = CreateObject<CHPPotion>(POTION_HP_PROTO,POTION_HP_PROTO);
	HPPotion1->SetPos(Vector2(200.f, 210.f));
	HPPotion1->SetTextureColorKey(255, 0, 255);

	// Tile Map
	CTileMap* TileMap = CreateMap<CTileMap>("TileMap");
	TileMap->LoadFile("Stage1.map");

	// WorldResolution is set to TileMap
	// if you want the other World Resolution Size instead of TileMap
	// Then, you have to set world resolution at the end  
	// or after TileMap
	// GetCamera()->SetWorldResolution(2000.f, 2000.f);

	// ScrollMap Size 1500.f, 1200.f
	// Scroll Map : Sky
	float	ScrollWidth = 1500.f - GetCamera()->GetResolution().x;
	float	ScrollHeight = 1200.f - GetCamera()->GetResolution().y;

	float	TileScrollMapWidth  = GetCamera()->GetWorldResolution().x - GetCamera()->GetResolution().x;
	float	TileScrollMapHeight = GetCamera()->GetWorldResolution().y - GetCamera()->GetResolution().y;

	/*
	CScrollMap* Map = CreateMap<CScrollMap>("ScrollMap");
	Map->SetSize(1280.f, 720.f);
	Map->SetTexture("Sky", TEXT("Sky.bmp")); // Sky.bmp : 1500, 1200
	Map->SetScrollRatio(ScrollWidth / TileScrollMapWidth, ScrollHeight / TileScrollMapHeight);
	Map->SetZOrder(0);

	// Scroll Map : Mountain
	Map = CreateMap<CScrollMap>("ScrollMap");
	ScrollWidth = 2048.f - GetCamera()->GetResolution().x;  // Mountain.bmp : 2048 * 2048
	ScrollHeight = 2048.f - GetCamera()->GetResolution().y;
	Map->SetSize(1280.f, 720.f);
	Map->SetTexture("Mountain", TEXT("Mountain.bmp"));
	Map->SetScrollRatio(ScrollWidth / TileScrollMapWidth, ScrollHeight / TileScrollMapHeight);
	Map->SetZOrder(1);
	Map->SetTextureColorKey(255, 0, 255);
	*/


	/*
	Scroll Map Infinite Loop

	GetCamera()->SetWorldResolution(300000.f, 120000.f);
	// ScrollMap Size 1500.f, 1200.f
	// Scroll Map : Sky
	CScrollMap* Map = CreateMap<CScrollMap>("ScrollMap");

	float	ScrollWidth = 1500.f - GetCamera()->GetResolution().x;
	float	ScrollHeight = 1200.f - GetCamera()->GetResolution().y;

	float	TileMapWidth = 3000.f - GetCamera()->GetResolution().x;
	float	TileMapHeight = 1200.f - GetCamera()->GetResolution().y;

	Map->SetSize(1280.f, 720.f);
	Map->SetTexture("ScrollBack", TEXT("Sky.bmp"));
	Map->SetLoop(true);
	Map->SetZOrder(0);

	*/

	// Monster
	// SetMonsterOnTileMap<CDuckMonster>(MONSTER_DUCK1_PROTO, MONSTER_DUCK2_PROTO, MONSTER_DUCK3_PROTO);

	return true;
}

void CMainScene::LoadAnimationSequence()
{
	SetPlayerAnimation();
	SetItemsAnimation();
	SetBulletsAnimation();
	SetCollideAnimation();
	SetMouseAnimation();
	SetPlayerAnimation();

	SetLevel1MonsterAnimation();
}

void CMainScene::LoadSound()
{
	// GetSceneResource()->LoadSound("BGM", true, "StartBGM", "MainBgm.mp3");
	// GetSceneResource()->SoundPlay("StartBGM");
	// GetSceneResource()->SetVolume("BGM", 1);

	GetSceneResource()->LoadSound("Effect", false, "Fire", "Fire1.wav");
	GetSceneResource()->SetVolume("Effect", 1);

	GetSceneResource()->LoadSound("Effect", false, "TextSound", "water-step-01.ogg");

	// Player --> run, dash
	GetSceneResource()->LoadSound("Player", false, "Run", "snow-step-1.ogg");
	GetSceneResource()->LoadSound("Player", false, "Dash", "snow-step-2.ogg");
}

void CMainScene::GoBackToWaitingScene()
{
	CSceneManager::GetInst()->CreateScene<CHomeScene>();
}
