#include "EffectGrenade.h"
#include "EffectExplodeTrace.h"
#include "EffectText.h"
#include "DamageFont.h"
#include "../GameManager.h"
#include "../Scene/Scene.h"
#include "../Scene/Camera.h"
#include "../Scene/SceneResource.h"
#include "Player.h"
#include "EffectHit.h"

CEffectGrenade::CEffectGrenade() :
	m_SpeedX(100.f),
	m_FallTime(0.f),
	m_Damage(100),
	m_Explode(false),
	m_ExplodeTime(0.f),
	m_ExplodeMaxTime(1.5f),
	m_ExplodeDist(100.f)
{
}

CEffectGrenade::CEffectGrenade(const CEffectGrenade& obj) :
	CGameObject(obj)
{
	m_SpeedX         = obj.m_SpeedX;
	m_FallTime       = 0.f;
	m_Damage         = 100;
	m_Explode        = false;
	m_ExplodeTime    = 0.f;
	m_ExplodeMaxTime = 3.f;
	m_ExplodeDist    = 100.f;
}

CEffectGrenade::~CEffectGrenade()
{
}

void CEffectGrenade::Start()
{
	CGameObject::Start();

	// Random Jump Velocity
	SetJumpVelocity(10.f);
	Jump();

	// EndNotify
	// SetAnimationEndNotify<CEffectGrenade>(GRENADE_ON, this, &CEffectGrenade::AnimationFinish);
}

bool CEffectGrenade::Init()
{
	if (!CGameObject::Init()) return false;

	SetPhysicsSimulate(true);
	return true;
}

void CEffectGrenade::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
	
	m_FallTime += DeltaTime;
	if (m_FallTime >= 0.7f)
		ChangeExplosionAnimation();

	if (m_Explode)
	{
		m_ExplodeTime += DeltaTime;
		if (m_ExplodeTime >= m_ExplodeMaxTime)
		{
			Explode();
			DrawExplodeTrace();
			Destroy();
		}
	}
}

void CEffectGrenade::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

void CEffectGrenade::Collision(float DeltaTime)
{
	CGameObject::Collision(DeltaTime);
}

void CEffectGrenade::Render(HDC hDC)
{
	// this
	CGameObject::Render(hDC);
}

CEffectGrenade* CEffectGrenade::Clone()
{
	return new CEffectGrenade(*this);
}

void CEffectGrenade::ChangeExplosionAnimation()
{
	m_IsGround = true;

	SetOffset(Vector2(-m_Size.x * 0.45f, 0));
	
	// Activate Animation
	AddAnimation(GRENADE_ON, false,  m_ExplodeMaxTime);

	m_Explode = true;
}

void CEffectGrenade::Explode()
{
	// Damage 200.f to Player If Dist from Grenade to Player is below 100.f 
	Vector2 GrenadePos = m_Pos ;
	CPlayer* Player = (CPlayer*)m_Scene->GetPlayer();

	Vector2 PlayerPos = Player->GetPos();
	Vector2 PlayerDir = Player->GetDir();

	float DistToPlayer = Distance(GrenadePos, Player->GetPos());
	if (DistToPlayer <= 150.f)
	{
		ExplodeHitPlayer(Player);
	}

	DrawExplodeTrace();
}

void CEffectGrenade::DrawExplodeTrace()
{
	CEffectExplodeTrace* GrenadeTrace = m_Scene->CreateObject<CEffectExplodeTrace>(
		"ExplodeAfter", 
		EXPLOSION_AFTER_PROTO,
		Vector2(m_Pos.x, m_Pos.y + m_Size.y * 0.7f));
}

void CEffectGrenade::ExplodeHitPlayer(class CPlayer* const Player)
{
	Vector2 PlayerPos = Player->GetPos();
	Vector2 PlayerDir = Player->GetDir();

	// Hit Effect 
	CEffectHit* Hit = m_Scene->CreateObject<CEffectHit>("HitEffect", EFFECT_HIT_PROTO,
		PlayerPos, Vector2(178.f, 164.f));
	m_Scene->GetSceneResource()->SoundPlay("Fire");

	// Hit 
	Player->SetHitDir(Vector2(-PlayerDir.x, -PlayerDir.y));
	Player->Hit();

	int Armor = Player->GetArmor();
	int FinalDamage = m_Damage - Armor;
	// Damage Font
	CDamageFont* DamageFont = m_Scene->CreateObject<CDamageFont>("DamageFont", DAMAGEFONT_PROTO, PlayerPos);
	DamageFont->SetDamageNumber(FinalDamage);

	Player->SetDamage(FinalDamage);
}
