
#include "Character.h"
#include "../Scene/Camera.h"
#include "../GameManager.h"
#include "../Scene/Scene.h"
#include "../Object/Gun.h"
#include "../Object/WallObject.h"
#include "../Collision/ColliderBox.h"

CCharacter::CCharacter() : 
	m_CharacterInfo{},
	m_GunEquipment{},
	m_CurrentGun(nullptr),
	m_HitEnable(false),
	m_HitTime(0.f),
	m_HitDir{}
{
	m_ObjType = EObject_Type::Character;
}

CCharacter::CCharacter(const CCharacter &obj) : CGameObject(obj)
{
	m_CharacterInfo = obj.m_CharacterInfo;
	m_HitEnable    = false;
	m_HitTime      = 0.f;
	m_HitDir = obj.m_HitDir;

	for (int i = 0; i < EGunClass::End; i++)
	{
		if (obj.m_GunEquipment[i])
		{
			m_GunEquipment[i] = obj.m_GunEquipment[i]->Clone();
			if (!m_CurrentGun && m_GunEquipment[i]->GetName() == obj.m_CurrentGun->GetName())
				m_CurrentGun = m_GunEquipment[i];
		}
	}
}

CCharacter::~CCharacter()
{
}


void CCharacter::Start()
{
	CGameObject::Start();
}

bool CCharacter::Init()
{
	if (!CGameObject::Init())
		return false;

	return true;
}

void CCharacter::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
	MoveWithinWorldResolution();
	CGameObject* CollideWall = WallCollisionCheck();
	if(CollideWall)
	{
		CWallObject* WallObj = (CWallObject*)CollideWall;
		Vector2 WallObjPos = WallObj->GetPos();
		RECT rcInter;

		CColliderBox* CharCollideBox = (CColliderBox*)GetColliderBox();
		CColliderBox* WallCollideBox = (CColliderBox*)WallObj->GetColliderBox();
		RectInfo CharRect    = CharCollideBox->GetInfo();
		RectInfo WallRect    = WallCollideBox->GetInfo();
		RectInfo Intersect   = GetInterCollideRect(CharRect,WallRect);
		float UpDownSpace    = Intersect.Bottom - Intersect.Top;
		float LeftRightSpace = Intersect.Right  - Intersect.Left;
		
		// 좌우 이동 
		if (LeftRightSpace <= 10.f)
		{
			// Going Right
			if (WallObjPos.x > m_Pos.x)
				m_Pos.x -= LeftRightSpace + 0.1f;
			// Going Left
			if (WallObjPos.x <= m_Pos.x)
				m_Pos.x += LeftRightSpace + 0.1f;
		}
		// 상하 이동
		if (UpDownSpace <= 10.f)
		{
			// Going Down
			if (WallObjPos.y > m_Pos.y)
				m_Pos.y -= UpDownSpace + 0.1f;
			// Going Up
			if (WallObjPos.y <= m_Pos.y)
				m_Pos.y += UpDownSpace + 0.1f;
		}
	}

	if (m_CurrentGun)
		m_CurrentGun->Update(DeltaTime);

	// MP, HP Max 조절
	if (m_CharacterInfo.MP > m_CharacterInfo.MPMax)
		m_CharacterInfo.MP = m_CharacterInfo.MPMax;

	if (m_CharacterInfo.HP > m_CharacterInfo.HPMax)
		m_CharacterInfo.HP = m_CharacterInfo.HPMax;

	// Hit 조절 
	if (m_HitTime >= 0.f) m_HitTime -= DeltaTime;
	if (m_HitEnable)
	{
		HitMove();
		if (m_HitTime < 0.f) HitEnd();
	}
}

void CCharacter::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
	if (m_CurrentGun)
		m_CurrentGun->PostUpdate(DeltaTime);
}

void CCharacter::Collision(float DeltaTime)
{
	CGameObject::Collision(DeltaTime);
}

void CCharacter::PrevRender()
{
	CGameObject::PrevRender();
	if (m_CurrentGun)
		m_CurrentGun->PrevRender();
}

void CCharacter::Render(HDC hDC)
{
	CGameObject::Render(hDC);
	if (m_CurrentGun)
		m_CurrentGun->Render(hDC);
}

CCharacter *CCharacter::Clone()
{
	return nullptr;
}

float CCharacter::SetDamage(float Damage)
{
	Damage = CGameObject::SetDamage(Damage);
	Damage -= m_CharacterInfo.Armor;
	if (Damage < 0) Damage = 0;
	m_CharacterInfo.HP -= (int)Damage;
	return Damage;
}

void CCharacter::SetScene(CScene* Scene)
{
	CGameObject::SetScene(Scene);
	// Guns
	for (int i = 0; i < EGunClass::End; i++)
	{
		if (m_GunEquipment[i])
		{
			CGun* Gun = m_GunEquipment[i];
			Gun->SetScene(Scene);
			Gun->SetOwner(this);
			if (!m_Scene->FindObject(Gun))
				m_Scene->PushObjectToScene(Gun);
		}
	}
}

void CCharacter::ChangeHitAnimation()
{
	if (!m_HitEnable) return;
}

void CCharacter::ChangeIdleAnimation()
{
	if (m_HitEnable) return;
}

void CCharacter::ChangeRunAnimation()
{
	if (m_HitEnable) return;
}

void CCharacter::ChangeMoveAnimation()
{
	if (m_HitEnable) return;
}

void CCharacter::ChangeDeathAnimation()
{
}

void CCharacter::Move(const Vector2& Dir)
{
	if (m_HitEnable) return;
	CGameObject::Move(Dir);
}

void CCharacter::Move(const Vector2& Dir, float Speed)
{
	if (m_HitEnable) return;
	CGameObject::Move(Dir, Speed);
}

void CCharacter::MoveWithinWorldResolution()
{
	float Space = 10.f;
	Vector2 WorldResolution = m_Scene->GetCamera()->GetWorldResolution();
	if (m_Pos.x <= Space)
		m_Pos.x = Space;
	if (m_Pos.x + m_Size.x >= WorldResolution.x - Space)
		m_Pos.x = WorldResolution.x - Space - m_Size.x;
	if (m_Pos.y <= Space)
		m_Pos.y = Space;
	if (m_Pos.y + m_Size.y >= WorldResolution.y - Space)
		m_Pos.y = WorldResolution.y - Space - m_Size.y;
}

bool CCharacter::CollisionCheck()
{
	auto iter = m_ColliderList.begin();
	auto iterEnd = m_ColliderList.end();
	for (; iter != iterEnd; ++iter)
	{
		if (!(*iter)->IsCollisionListEmpty())
		{
			return true;
		}
	}
	return false;
}

bool CCharacter::ObstacleCollisionCheck() const
{
	auto iter = m_ColliderList.begin();
	auto iterEnd = m_ColliderList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->DidCollideWithObstacles())
			return true;
	}

	return false;
}

CGameObject* CCharacter::MonsterCollisionCheck()
{
	auto iter = m_ColliderList.begin();
	auto iterEnd = m_ColliderList.end();
	for (; iter != iterEnd; ++iter)
	{
		CGameObject* Monster = (*iter)->IsCollisionWithMonster();
		if (Monster)
		{
			return Monster;
		}
	}
	return nullptr;
}

CGameObject* CCharacter::WallCollisionCheck()
{
	auto iter = m_ColliderList.begin();
	auto iterEnd = m_ColliderList.end();
	for (; iter != iterEnd; ++iter)
	{
		CGameObject* WallCollide = (*iter)->IsCollisionWithWall();
		if (WallCollide) return WallCollide;
	}
	return nullptr;
}

void CCharacter::CollideBounceBack(Vector2 Dir)
{
	Vector2 OppDir = Dir;
	OppDir.Normalize();
	SetHitDir(OppDir);
	Hit();
}

CCollider* CCharacter::GetColliderBox()
{
	auto iter = m_ColliderList.begin();
	auto iterEnd = m_ColliderList.end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetName() == "Body")
			return (*iter);
	}
	return nullptr;
}

RectInfo CCharacter::GetInterCollideRect(RectInfo Rect1, RectInfo Rect2)
{
	RectInfo Intersect;
	// max among left 
	Intersect.Left  = Rect1.Left > Rect2.Left ? Rect1.Left : Rect2.Left;
	// Min among Right 
	Intersect.Right = Rect1.Right < Rect2.Right ? Rect1.Right : Rect2.Right;
	// Max among y1
	Intersect.Top   = Rect1.Top > Rect2.Top ? Rect1.Top : Rect2.Top;
	// Min among y2
	Intersect.Bottom = Rect1.Bottom < Rect2.Bottom ? Rect1.Bottom : Rect2.Bottom;
	return Intersect;
}

void CCharacter::SetHitDir(Vector2 Dir)
{
	m_HitDir = Dir;
	m_HitDir.Normalize();
}

void CCharacter::Hit()
{
	m_HitTime = HIT_TIME;
	m_HitEnable = true;
	m_Pos += m_HitDir * m_Size.Length() * 0.1f;
	ChangeHitAnimation();
}
void CCharacter::HitMove()
{
	Vector2	CurrentMove = m_HitDir * HIT_SPEED * CGameManager::GetInst()->GetDeltaTime() * m_TimeScale;
	m_Velocity += CurrentMove;
	m_Pos += CurrentMove;
}

void CCharacter::HitEnd()
{
	m_HitEnable = false;
	ChangeIdleAnimation();
}

void CCharacter::CharacterDestroy()
{
	Destroy();
	for (int i = 0; i < EGunClass::End; i++)
	{
		if (m_GunEquipment[i])
			m_GunEquipment[i]->Destroy();
	}
}

void CCharacter::SetInitGun(CGun* Gun)
{
	EGunClass GunClass = Gun->GetGunClass();
	m_GunEquipment[GunClass] = Gun;
	m_CurrentGun = Gun;
}

CGun* CCharacter::Equip(CGun* Gun)
{
	EGunClass GunClass = EGunClass::Light;
	switch (Gun->GetGunClass())
	{
	case EGunClass::Light :
		GunClass = Light;
		break;
	case EGunClass::Medium:
		GunClass = Medium;
		break;
	case EGunClass::Heavy :
		GunClass = Heavy;
		break;
	}
	// Chnage Equipment 
	CGun*ExistingGun = m_GunEquipment[GunClass];
	m_GunEquipment[GunClass] = Gun;
	// Set Current Gun
	m_CurrentGun = m_GunEquipment[GunClass];

	// Set Owner, Pos 
	Gun->SetOwner(this);
	// 아래의 코드가 반드시 있어야 한다... 왜지 ?
	// Gun->SetPos(m_Pos);
	// Gun->SetOffset(m_Size.x * 0.1, -m_Size.y * 0.3);
	// Gun->SetPos(m_Size.x * 0.1, -m_Size.y * 0.3);

	return ExistingGun;
}


