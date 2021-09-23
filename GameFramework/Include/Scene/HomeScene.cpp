#include "HomeScene.h"
#include "SceneResource.h"
#include "../Object/Player.h"
#include "../Scene/Camera.h"
#include "../Object/StageDoor.h"
#include "../Object/EffectHit.h"
#include "../UI/UICharacterStateHUD.h"

CHomeScene::CHomeScene()
{
}

CHomeScene::~CHomeScene()
{
}

bool CHomeScene::Init()
{
	// Sound 
	LoadSound();

	// Animation
	LoadAnimationSequence();

	// Objects
	CPlayer* Player = CreateObject<CPlayer>("Player");
	Player->SetCharacterInfo(60, 10, NORMAL_MONSTER_HP_MAX, NORMAL_MONSTER_MP_MAX,
		1, 100, 100, 100, NORMAL_ATTACK_DISTANCE);

	SetPlayer(Player);
	GetCamera()->SetTarget(Player);
	GetCamera()->SetTargetPivot(0.5f, 0.5f);

	Vector2 WorldResolution = m_Camera->GetWorldResolution();
	CStageDoor* StageDoor_One = CreateObject<CStageDoor>("StageDoor", 
		Vector2(300.f + rand() % 700, 30.f + rand() % 100),
		Vector2(50.f,50.f));
	StageDoor_One->SetDoorStageType(EDoorStage_Type::Stage_One);


	// Prototypes
	CEffectHit* EffectPrototype = CreatePrototype<CEffectHit>("HitEffect");

	// Teleport
	CTeleportMouse* TeleportMousePrototype = CreatePrototype<CTeleportMouse>("TeleportMouse");
	CBullet* PlayerBullet = CreatePrototype<CBullet>("PlayerBullet");
	CBullet* SlowMotionAttackBullet = CreatePrototype<CBullet>("SkillSlowMotionAttackBullet");

	CCollider* Collider = PlayerBullet->FindCollider("Body");
	if (Collider)
		Collider->SetCollisionProfile("PlayerAttack");

	// Window
	CUICharacterStateHUD* StateWindow = CreateUIWindow<CUICharacterStateHUD>("CharacterStateHUD");

	return true;
}

void CHomeScene::LoadAnimationSequence()
{
	// Player ---
	GetSceneResource()->CreateAnimationSequence("LucidNunNaRightIdle",
		"LucidNunNaRightIdle", TEXT("Player/Right/astand.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaRightIdle",
		255, 0, 255);

	for (int i = 0; i < 6; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaRightIdle",
			i * 82.f, 0.f, 82.f, 73.f);
	}

	GetSceneResource()->CreateAnimationSequence("LucidNunNaRightWalk",
		"LucidNunNaRightWalk", TEXT("Player/Right/awalk.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaRightWalk",
		255, 0, 255);

	for (int i = 0; i < 4; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaRightWalk",
			i * 85.f, 0.f, 85.f, 75.f);
	}

	GetSceneResource()->CreateAnimationSequence("LucidNunNaRightAttack",
		"LucidNunNaRightAttack", TEXT("Player/Right/aswing.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaRightAttack",
		255, 0, 255);

	for (int i = 0; i < 3; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaRightAttack",
			i * 176.f, 0.f, 176.f, 89.f);
	}

	// Slow Motion Skill
	GetSceneResource()->CreateAnimationSequence("SkillSlowMotionAttack",
		"SkillSlowMotionAttack", TEXT("Player/Right/ashoot1.bmp"));
	GetSceneResource()->SetTextureColorKey("SkillSlowMotionAttack",
		255, 0, 255);

	for (int i = 0; i < 3; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("SkillSlowMotionAttack",
			i * 70.f, 0.f, 70.f, 81.f);
	}

	// �ڷ���Ʈ ĳ����
	/* 1��° ����
		GetSceneResource()->CreateAnimationSequence("LucidNunNaTeleport",
			"LucidNunNaTeleport", TEXT("Player/teleport.bmp"));

		GetSceneResource()->SetTextureColorKey("LucidNunNaTeleport",
			255, 0, 255);

		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				GetSceneResource()->AddAnimationFrameData("LucidNunNaTeleport",
					j * 240.f, i * 320.f, 240.f, 320.f);
			}
		}
	*/
	// 3��° ����
	GetSceneResource()->CreateAnimationSequence("LucidNunNaTeleport",
		"LucidNunNaTeleport", TEXT("Player/teleport2.bmp"));

	// �ӽ������� SetTexture Ű ���� ( ���ȭ����� �Ͼ�� )
	// GetSceneResource()->SetTextureColorKey("LucidNunNaTeleport",
		// 255, 0, 255);

	for (int i = 0; i < 11; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaTeleport",
			i * 57.3f, 0.f, 57.3f, 78.f);
	}


	// ���� ���� �⺻ �ִϸ��̼� 
	GetSceneResource()->CreateAnimationSequence("LucidNunNaLeftIdle",
		"LucidNunNaLeftIdle", TEXT("Player/Left/astand_left.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaLeftIdle",
		255, 0, 255);

	for (int i = 0; i < 6; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaLeftIdle",
			i * 82.f, 0.f, 82.f, 73.f);
	}

	// Left  Walk
	GetSceneResource()->CreateAnimationSequence("LucidNunNaLeftWalk",
		"LucidNunNaLeftWalk", TEXT("Player/Left/awalk_left.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaLeftWalk",
		255, 0, 255);

	for (int i = 0; i < 4; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaLeftWalk",
			i * 85.f, 0.f, 85.f, 75.f);
	}

	// Run
	GetSceneResource()->CreateAnimationSequence("LucidNunNaLeftRun",
		"LucidNunNaLeftRun", TEXT("Player/Left/alert_left.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaLeftRun",
		255, 0, 255);

	for (int i = 0; i < 3; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaLeftRun",
			i * 59.f, 0.f, 59.f, 87.f);
	}

	// right run
	GetSceneResource()->CreateAnimationSequence("LucidNunNaRightRun",
		"LucidNunNaRightRun", TEXT("Player/Right/alert.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaRightRun",
		255, 0, 255);

	for (int i = 0; i < 3; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaRightRun",
			i * 59.f, 0.f, 59.f, 87.f);
	}

	// Stun
	GetSceneResource()->CreateAnimationSequence("LucidNunNaStun",
		"LucidNunNaStun", TEXT("Player.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaStun",
		255, 0, 255);
	for (int i = 0; i < 3; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaStun",
			i * 32.f, 0.f, 32.f, 32.f);
	}

	// Target Attack 
	GetSceneResource()->CreateAnimationSequence("LucidNunNaTargetAttack",
		"LucidNunNaTargetAttack", TEXT("Player/TargetAttack.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaTargetAttack",
		255, 0, 255);

	for (int i = 0; i < 3; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaTargetAttack",
			i * 192.f, 0.f, 192.f, 192.f);
	}

	// Destory
	GetSceneResource()->CreateAnimationSequence("LucidNunNaLeftDeath",
		"LucidNunNaLeftDeath", TEXT("Player/Left/dead.bmp"));
	GetSceneResource()->SetTextureColorKey("LucidNunNaLeftDeath",
		255, 0, 255);
	for (int i = 0; i < 1; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaLeftDeath",
			i * 59.f, 0.f, 59.f, 68.f);
	}

	GetSceneResource()->CreateAnimationSequence("LucidNunNaRightDeath",
		"LucidNunNaRightDeath", TEXT("Player/Right/Right_dead.bmp"));

	GetSceneResource()->SetTextureColorKey("LucidNunNaRightDeath",
		255, 0, 255);

	for (int i = 0; i < 3; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("LucidNunNaRightDeath",
			i * 59.f, 0.f, 59.f, 68.f);
	}

	// Door ---
	GetSceneResource()->CreateAnimationSequence("StageDoor",
		"StageDoor", TEXT("DoorToNextStage.bmp"));
	GetSceneResource()->SetTextureColorKey("StageDoor",
		255, 0, 255);

	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 5; j++)
		{
			GetSceneResource()->AddAnimationFrameData("StageDoor",
				j * 192.f, i *192.f, 192.f, 192.f);
		}
	}

	// �׿� �ִϸ��̼� ---
	// �浹 ȿ�� �ִϸ��̼� 
	GetSceneResource()->CreateAnimationSequence("HitRight",
		"HitRight", TEXT("Hit2.bmp"));
	GetSceneResource()->SetTextureColorKey("HitRight",
		255, 0, 255);

	for (int i = 0; i < 6; ++i)
	{
		GetSceneResource()->AddAnimationFrameData("HitRight",
			i * 178.f, 0.f, 178.f, 164.f);
	}

	// �Ѿ�
	GetSceneResource()->CreateAnimationSequence("Bullet",
		"Bullet", TEXT("Smoke.bmp"));
	GetSceneResource()->SetTextureColorKey("Bullet",
		255, 0, 255);
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			GetSceneResource()->AddAnimationFrameData("Bullet",
				j * 92.f, i * 92.f, 92.f, 92.f);
		}
	}

	// �ڸ���Ʈ ���콺 
	GetSceneResource()->CreateAnimationSequence("TeleportMouseDisplay",
		"TeleportMouseDisplay", TEXT("Mouse/h.bmp"));
	GetSceneResource()->SetTextureColorKey("TeleportMouseDisplay", 255, 0, 255);
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			GetSceneResource()->AddAnimationFrameData("TeleportMouseDisplay",
				j * 61.f, i * 61.f, 61.f, 61.f);
		}
	}
}

void CHomeScene::LoadSound()
{
	GetSceneResource()->LoadSound("BGM", true, "StartBGM", "MainBgm.mp3");
	GetSceneResource()->SoundPlay("StartBGM");
	GetSceneResource()->SetVolume("StartBGM", 1);


	GetSceneResource()->LoadSound("Effect", false, "Fire", "Fire1.wav");
	GetSceneResource()->SetVolume("Effect", 1);

	GetSceneResource()->LoadSound("Effect", false, "TextSound", "water-step-01.ogg");

	// Player --> run, dash
	GetSceneResource()->LoadSound("Player", false, "Run", "snow-step-1.ogg");
	GetSceneResource()->LoadSound("Player", false, "Dash", "snow-step-2.ogg");
}

// DoorToNextStage
