
#include "Scene.h"
#include "SceneResource.h"
#include "SceneManager.h"
#include "SceneCollision.h"
#include "Camera.h"
#include "../Object/EffectHit.h"
#include "../Object/DamageFont.h"
#include "../Map/MapBase.h"

CScene::CScene()
{
	m_Resource = new CSceneResource;
	m_Collision = new CSceneCollision;

	m_Collision->m_Scene = this;

	m_RenderCount = 0;
	m_RenderCapacity = 100;
	m_RenderArray = new CGameObject * [m_RenderCapacity];

	m_UICount = 0;
	m_UICapacity = 10;
	m_UIArray = new CUIWindow * [m_UICapacity];

	m_Camera = new CCamera;

	m_Camera->Init();


}

CScene::~CScene()
{
	SAFE_DELETE(m_Camera);
	SAFE_DELETE_ARRAY(m_RenderArray);

	for (int i = 0; i < m_UICount; ++i)
	{
		SAFE_RELEASE(m_UIArray[i]);
	}
	SAFE_DELETE_ARRAY(m_UIArray);

	{
		auto iter = m_MapList.begin();
		auto iterEnd = m_MapList.end();
		for (; iter != iterEnd; ++iter)
		{
			SAFE_DELETE((*iter));
		}
		m_MapList.clear();
	}

	m_ObjList.clear();
	m_mapPrototype.clear();
	m_Player = nullptr; // m_Player = nullptr;

	SAFE_DELETE(m_Collision);
	SAFE_DELETE(m_Resource);
}

CSceneResource* CScene::GetSceneResource() const
{
	return m_Resource;
}

CSceneCollision* CScene::GetSceneCollision() const
{
	return m_Collision;
}

CCamera* CScene::GetCamera() const
{
	return m_Camera;
}

CGameObject* CScene::FindObject(const std::string& Name)
{
	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetName() == Name)
			return *iter;
	}

	return nullptr;
}

CGameObject* CScene::FindClosestMonsterToPlayer(Vector2 PlayerPos)
{
	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();
	float   MinLength = (float)INT_MAX;
	float   FLOAT_MAX = (float)INT_MAX;
	CGameObject* ClosestObj = nullptr;

	for (; iter != iterEnd; ++iter)
	{
		// Monster Type이 아니면 건너뛴다 
		if ((*iter)->GetObjType() != EObject_Type::Monster) continue;

		float LengthToObj = Vector2((*iter)->GetPos() - PlayerPos).Length();
		if (LengthToObj < MinLength)
		{
			MinLength = LengthToObj;
			ClosestObj = (*iter);
		}
	}
	if (MinLength == FLOAT_MAX)
		return nullptr;
	return ClosestObj;
}

void CScene::DestroyAllAttackObjects()
{
	auto iter = m_ObjList.begin();
	auto iterEnd = m_ObjList.end();
	for (; iter != iterEnd;++iter)
	{
		if ((*iter)->GetObjType() == EObject_Type::Bullet)
		{
			Vector2 ObjPos = (*iter)->GetPos();
			(*iter)->Destroy();
			CEffectHit* Hit = CreateObject<CEffectHit>("HitEffect", "HitEffect",
				ObjPos, Vector2(178.f, 164.f));
			GetSceneResource()->SoundPlay("Fire");
		}

	}
}

CGameObject* CScene::SetPlayer(const std::string& Name)
{
	CGameObject* Player = FindObject(Name);
	return SetPlayer(Player);
}

CGameObject* CScene::SetPlayer(CGameObject* Player)
{
	if (!Player) return nullptr;

	// Scene내의 player 세팅
	m_Player = Player;

	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == Player)
		{
			m_ObjList.erase(iter);
			break;
		}
	}
	return m_Player;
}

bool CScene::Init()
{
	return true;
}

bool CScene::Update(float DeltaTime)
{
	if (m_Player)
	{
		m_Player->Update(DeltaTime);
	}

	{
		auto	iter = m_ObjList.begin();
		auto	iterEnd = m_ObjList.end();

		for (; iter != iterEnd;)
		{
			if (!(*iter)->IsActive())
			{
				iter = m_ObjList.erase(iter);
				iterEnd = m_ObjList.end();
				continue;
			}
			(*iter)->Update(DeltaTime * (*iter)->m_TimeScale);
			++iter;
		}
	}

	{
		for (int i = 0; i < m_UICount;)
		{
			if (!m_UIArray[i]->IsActive())
			{
				--m_UICount;
				for (int j = i; j < m_UICount; ++j)
				{
					m_UIArray[j] = m_UIArray[j + 1];
				}
				continue;
			}

			else if (!m_UIArray[i]->GetVisibility())
			{
				++i;
				continue;
			}

			m_UIArray[i]->Update(DeltaTime);
			++i;
		}
	}

	return false;
}

bool CScene::PostUpdate(float DeltaTime)
{
	if (m_Player)
	{
		m_Player->PostUpdate(DeltaTime);
	}
	{
		std::list<CSharedPtr<CGameObject>>::iterator	iter = m_ObjList.begin();
		std::list<CSharedPtr<CGameObject>>::iterator	iterEnd = m_ObjList.end();

		for (; iter != iterEnd;)
		{
			if (!(*iter)->IsActive())
			{
				iter = m_ObjList.erase(iter);
				iterEnd = m_ObjList.end();
				continue;
			}

			(*iter)->PostUpdate(DeltaTime * (*iter)->m_TimeScale);
			++iter;
		}
	}

	{
		for (int i = 0; i < m_UICount;)
		{
			if (!m_UIArray[i]->IsActive())
			{
				--m_UICount;

				for (int j = i; j < m_UICount; ++j)
				{
					m_UIArray[j] = m_UIArray[j + 1];
				}

				continue;
			}

			else if (!m_UIArray[i]->GetVisibility())
			{
				++i;
				continue;
			}

			m_UIArray[i]->PostUpdate(DeltaTime);
			++i;
		}
	}

	m_Camera->Update(DeltaTime);

	// TileMap Update는 PostUpdate에서 하자.
	// 단, Render는 반드시 Map을 먼저 시켜야 한다
	// 왜냐하면, 여기서 카메라가 update 되기 때문이다 
	{
		auto	iter = m_MapList.begin();
		auto	iterEnd = m_MapList.end();

		for (; iter != iterEnd;)
		{
			if (!(*iter)->IsActive())
			{
				SAFE_DELETE((*iter));
				iter = m_MapList.erase(iter);
				iterEnd = m_MapList.end();
				continue;
			}

			else if (!(*iter)->IsEnable())
			{
				++iter;
				continue;
			}

			(*iter)->Update(DeltaTime);
			++iter;
		}
	}

	{
		auto	iter = m_MapList.begin();
		auto	iterEnd = m_MapList.end();

		for (; iter != iterEnd;)
		{
			if (!(*iter)->IsActive())
			{
				SAFE_DELETE((*iter));
				iter = m_MapList.erase(iter);
				iterEnd = m_MapList.end();
				continue;
			}

			else if (!(*iter)->IsEnable())
			{
				++iter;
				continue;
			}

			(*iter)->PostUpdate(DeltaTime);
			++iter;
		}
	}

	return false;
}

bool CScene::Collision(float DeltaTime)
{
	if (m_Player)
	{
		m_Player->Collision(DeltaTime);
	}
	{
		std::list<CSharedPtr<CGameObject>>::iterator	iter = m_ObjList.begin();
		std::list<CSharedPtr<CGameObject>>::iterator	iterEnd = m_ObjList.end();

		for (; iter != iterEnd;)
		{
			if (!(*iter)->IsActive())
			{
				iter = m_ObjList.erase(iter);
				iterEnd = m_ObjList.end();
				continue;
			}

			(*iter)->Collision(DeltaTime * (*iter)->m_TimeScale);
			++iter;
		}
	}

	{
		if (m_UICount >= 2)
		{
			qsort(m_UIArray, (size_t)m_UICount, sizeof(CUIWindow*),
				CScene::SortZOrder);
		}

		for (int i = 0; i < m_UICount;)
		{
			if (!m_UIArray[i]->IsActive())
			{
				--m_UICount;

				for (int j = i; j < m_UICount; ++j)
				{
					m_UIArray[j] = m_UIArray[j + 1];
				}

				continue;
			}

			else if (!m_UIArray[i]->GetVisibility())
			{
				++i;
				continue;
			}

			m_UIArray[i]->Collision(DeltaTime);

			m_Collision->AddUIWindow(m_UIArray[i]);

			++i;
		}
	}

	m_Collision->CollisionMouse(DeltaTime);
	m_Collision->Collision(DeltaTime);

	return false;
}

bool CScene::Render(HDC hDC)
{
	// ZOrder에 따라 정렬한다
	if (m_MapList.size() >= 2)
	{
		qsort(&m_MapList[0], (size_t)m_MapList.size(),
			sizeof(CMapBase*),CScene::SortZOrderMap);
	}
	{
		auto	iter = m_MapList.begin();
		auto	iterEnd = m_MapList.end();

		for (; iter != iterEnd;)
		{
			if (!(*iter)->IsActive())
			{
				SAFE_DELETE((*iter));
				iter = m_MapList.erase(iter);
				iterEnd = m_MapList.end();
				continue;
			}

			else if (!(*iter)->IsEnable())
			{
				++iter;
				continue;
			}

			(*iter)->Render(hDC);
			++iter;
		}
	}

	if (m_Player)
	{
		m_Player->PrevRender();
	}

	{
		std::list<CSharedPtr<CGameObject>>::iterator	iter = m_ObjList.begin();
		std::list<CSharedPtr<CGameObject>>::iterator	iterEnd = m_ObjList.end();

		for (; iter != iterEnd;)
		{
			if (!(*iter)->IsActive())
			{
				iter = m_ObjList.erase(iter);
				iterEnd = m_ObjList.end();
				continue;
			}

			(*iter)->PrevRender();

			if (!(*iter)->IsCull())
			{
				if (m_RenderCount == m_RenderCapacity)
				{
					m_RenderCapacity *= 2;

					CGameObject** Array = new CGameObject * [m_RenderCapacity];

					memcpy(Array, m_RenderArray, sizeof(CGameObject*) * m_RenderCount);

					delete[]	m_RenderArray;

					m_RenderArray = Array;
				}

				m_RenderArray[m_RenderCount] = *iter;
				++m_RenderCount;
			}

			++iter;
		}
	}

	if (m_RenderCount == m_RenderCapacity)
	{
		m_RenderCapacity *= 2;

		CGameObject** Array = new CGameObject * [m_RenderCapacity];

		memcpy(Array, m_RenderArray, sizeof(CGameObject*) * m_RenderCount);

		delete[]	m_RenderArray;

		m_RenderArray = Array;
	}

	if (m_Player)
	{
		if (!m_Player->IsCull())
		{
			m_RenderArray[m_RenderCount] = m_Player;
			++m_RenderCount;
		}
	}

	// 출력 목록을 정렬한다.
	qsort(m_RenderArray, (size_t)m_RenderCount, sizeof(CGameObject*),
		CScene::SortY);
	
	for (int i = 0; i < m_RenderCount; ++i)
	{
		m_RenderArray[i]->Render(hDC);
	}

	m_RenderCount = 0;

	{
		for(int i = 0 ;i < m_UICount; )
		{
			if (!m_UIArray[i]->IsActive())
			{
				--m_UICount;
				for (int j = i; j < m_UICount; ++j)
				{
					m_UIArray[j] = m_UIArray[j + 1];
				}
				continue;
			}
			++i;
		}

		for (int i = m_UICount - 1; i >= 0;)
		{
			if (!m_UIArray[i]->GetVisibility())
			{
				--i;
				continue;
			}
			m_UIArray[i]->Render(hDC);
			--i;
		}
	}

	return false;
}

int CScene::SortY(const void* Src, const void* Dest)
{
	CGameObject* SrcObj = *(CGameObject**)Src;
	CGameObject* DestObj = *(CGameObject**)Dest;

	// Bottom 값을 구한다.
	float	SrcY = SrcObj->GetBottom();
	float	DestY = DestObj->GetBottom();

	if (SrcY < DestY)
		return -1;
	// 여기서는 Y가 클수록, 아래에 있다는 것이고
	// Y가 작을 수록, 위에 있다는 것이다
	// 우리는 아래에 있는 Y를 화면 앞에다가 그리고 싶은 것
	// 즉, Y값이 큰애를 화면 앞에다가 그리고 싶은 것
	// 따라서, 먼저 SortY를 통해, Y가 작은 애들, 
	// 즉, 더 위에있는 애들이 앞에오게 정렬한 다음
	// Render에서, 배열 앞에서부터 그림으로써
	// 배열 상 뒤로 갈수록, 아래에 위치해있는 애들을
	// 나중에 그리게 하고, 이를 통해, 화면 앞쪽에 그릴 수 있게 설정하는 것이다 
	else if (SrcY > DestY)
		return 1;

	return 0;
}

int CScene::SortZOrder(const void* Src, const void* Dest)
{
	CUIWindow* SrcObj = *(CUIWindow**)Src;
	CUIWindow* DestObj = *(CUIWindow**)Dest;

	// Bottom 값을 구한다.
	int	SrcZ = SrcObj->GetZOrder();
	int	DestZ = DestObj->GetZOrder();

	if (SrcZ > DestZ)
		return -1;

	else if (SrcZ < DestZ)
		return 1;

	return 0;
}

int CScene::SortZOrderMap(const void* Src, const void* Dest)
{
	CMapBase* SrcObj  = *(CMapBase**)Src;
	CMapBase* DestObj = *(CMapBase**)Dest;

	// Bottom 값을 구한다
	int SrcZ  = SrcObj->GetZOrder();
	int DestZ = DestObj->GetZOrder();

	if (SrcZ > DestZ) return 1;
	else if (SrcZ < DestZ) return -1;

	return 0;
}

CGameObject* CScene::FindPrototype(const std::string& Name)
{
	auto	iter = m_mapPrototype.find(Name);

	if (iter == m_mapPrototype.end())
		return nullptr;

	return iter->second;
}

CPlayer* CScene::CreatePlayer(const std::string& Name, const Vector2& Pos, const Vector2& Size)
{
	CPlayer* Player = (CPlayer*)CSceneManager::GetInst()->GetPlayer();

	if (!Player)
	{
		Player = CreateObject<CPlayer>("Player",Pos,Size);
		Player->SetCharacterInfo(100, 20, PLAYER_INIT_HP, PLAYER_INIT_MP, 1, 1, 1, 
			NORMAL_SPEED, NORMAL_ATTACK_DISTANCE, NORMAL_ATTACK_DISTANCE);
		return Player;
	}

	Player->SetScene(this);
	Player->SetPos(Pos);
	Player->SetSize(Size);
	Player->SetName(Name);

	{
		auto iter    = Player->m_ColliderList.begin();
		auto iterEnd = Player->m_ColliderList.end();
		for (; iter != iterEnd; ++iter)
		{
			(*iter)->SetScene(this);
		}
	}

	{
		auto iter = Player->m_WidgetComponentList.begin();
		auto iterEnd = Player->m_WidgetComponentList.end();
		for (; iter != iterEnd; ++iter)
		{
			(*iter)->SetScene(this);
		}
	}

	m_ObjList.push_back(Player);

	return Player;
}
