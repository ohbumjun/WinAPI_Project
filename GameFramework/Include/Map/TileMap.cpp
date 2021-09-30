#include "TileMap.h"
#include "../Scene/Scene.h"
#include "../Scene/Camera.h"

CTileMap::CTileMap() :
    m_TileCountX(0),
    m_TileCountY(0),
    m_StartX(-1),
    m_StartY(-1),
    m_EndX(-1),
    m_EndY(-1)
{
}

CTileMap::~CTileMap()
{
    size_t Size = m_vecTile.size();
    for (size_t i = 0; i < Size; i++)
    {
        SAFE_DELETE(m_vecTile[i]);
    }
    m_vecTile.clear();
}

bool CTileMap::CreateTile(int CountX, int CountY, const Vector2& TileSize)
{
    // 가로, 세로 크기 
    m_TileCountX = CountX;
    m_TileCountY = CountY;
    m_TileSize = TileSize;

    // 전체 타일을 다 날려버린다 
    // 새롭게 Map을 만드는 과정이기 때문에
    // 기존에 세팅된 타일들을 정리해주는 것이다 
    {
        auto iter = m_vecTile.begin();
        auto iterEnd = m_vecTile.end();
        for (; iter != iterEnd; ++iter)
        {
            SAFE_DELETE((*iter));
        }
        m_vecTile.clear();
    }

    // 가로, 세로 개수만큼 반복돌릴 것이다
    // Y : 행
    // X : 열 
    // 타일 전체 크기 
    m_Size = m_TileSize * Vector2((float)m_TileCountX, (float)m_TileCountY);

    // TileMap 크기로, Camera 의 크기를 세팅한다
    // 이렇게 하면, TileMap이 만들어진
    // 영역 안에서 움직이게 될 것이다 
    CCamera* Camera = m_Scene->GetCamera();
    Camera->SetWorldResolution(m_Size);

    for (int i = 0; i < m_TileCountY; ++i)
    {
        for (int j = 0; j < m_TileCountX; ++j)
        {
            CTile* Tile = new CTile;
            Tile->Init();

            // 현재 Tile Map 위치
            Vector2 Pos = Vector2((float)j,
                (float)i) * m_TileSize;
            
            // j : 가로 idx ( 열 )
            // i : 세로 idx ( 행 )
            // i*m_TileCountX+j : 총 idx 
            Tile->SetTileInfo(Pos,m_TileSize,
                j,i,
                i*m_TileCountX+j, m_TileTexture);
            m_vecTile.push_back(Tile);
        }
    }

    for (int i = 0; i < m_TileCountY; ++i)
    {
        for (int j = 0; j < m_TileCountX; ++j)
        {
            // Texture만 전체 세팅 해준다 
            m_vecTile[i * m_TileCountX + j]->SetTileTexture(m_TileTexture);
        }
    }

    return true;
}

bool CTileMap::SetTileTexture(CTexture* Texture)
{
    m_TileTexture = Texture;
    for (int i = 0; i < m_TileCountY; i++)
    {
        for (int j = 0; j < m_TileCountX; j++)
        {
            // Texture만 전체 세팅 해준다 
            m_vecTile[i * m_TileCountX + j]->SetTileTexture(m_TileTexture);
        }
    }
    return true;
}

bool CTileMap::SetTileTexture(CTexture* Texture, int IndexX, int IndexY)
{
    m_TileTexture = Texture;
    m_vecTile[IndexY * m_TileCountX + IndexX]->SetTileTexture(m_TileTexture);
    return true;
}

void CTileMap::Start()
{
    CMapBase::Start();
}

bool CTileMap::Init()
{
    if (!CMapBase::Init()) return false;
    return true;
}

void CTileMap::Update(float DeltaTime)
{
    CMapBase::Update(DeltaTime);

    if (!m_vecTile.empty())
    {
        // 전체 TileMap 들 중에서
        // 일부는 보이고, 일부는 안보일 것이다
        // 화면상에서 camera를 통해서, 보이는 것과 안보이는 것을 구분했듯이
        // TileMap 상에서도, 보이는 Tile과 안보이는 Tile을 구분할 것이다
        // 그런데 만약 타일 개수가 100* 100 = 10000 개라면
        // 이중 반복문 다 돌면서, 각 타일을 확인하는 것은
        // 시간이 매우 오래 걸린다
    
        // 간단한 방법은, 해당 x,y 위치를 tile size로 나누면
        // 해당 Tile의 idxX, idxY가 나오게 될 것이다
        CCamera* Camera    = m_Scene->GetCamera();
        Vector2 CameraPos  = Camera->GetPos();
        Vector2 Resolution = Camera->GetResolution();

        // m_StartX,m_StartY : 현재 카메라 화면 내에 보이는 첫번째 타일의 x,y Idx
        // m_EndX,m_EndY : 현재 카메라 화면 내에 보이는 마지막 타일의 x,y Idx
        m_StartX = (int)(CameraPos.x / m_TileSize.x);
        m_StartY = (int)(CameraPos.y / m_TileSize.y);
        m_EndX   = (int)((CameraPos.x + Resolution.x )/ m_TileSize.x);
        m_EndY   = (int)((CameraPos.y + Resolution.y )/ m_TileSize.y);

        // idx 범위 조정 
        m_StartX = m_StartX < 0 ? 0 : m_StartX;
        m_StartY = m_StartY < 0 ? 0 : m_StartY;
        m_EndX   = m_EndX >= m_TileCountX ? m_TileCountX - 1 : m_EndX;
        m_EndY   = m_EndY >= m_TileCountY ? m_TileCountY - 1 : m_EndY;

        // 매 Frame 마다 m_StartX,Y m_EndX,Y 는 Update 에서 한번씩만 구한다 
        for (int i = m_StartY; i <= m_EndY; ++i)
        {
            for (int j = m_StartX; j <= m_EndX; ++j)
            {
                m_vecTile[i * m_TileCountX + j]->Update(DeltaTime);
            }
        }

    }
}

void CTileMap::PostUpdate(float DeltaTime)
{
    CMapBase::PostUpdate(DeltaTime);
    if (!m_vecTile.empty())
    {
        // Update 에서 아직 세팅이 안됐더라면 
        if (m_StartY == -1 || m_StartX == -1 || m_EndX == -1 || m_EndY == -1)
            return;
        for (int i = m_StartY; i <= m_EndY; ++i)
        {
            for (int j = m_StartX; j <= m_EndX; ++j)
            {
                m_vecTile[i * m_TileCountX + j]->PostUpdate(DeltaTime);
            }
        }
    }
}

void CTileMap::PrevRender()
{
    CMapBase::PrevRender();
    if (!m_vecTile.empty())
    {
        // Update 에서 아직 세팅이 안됐더라면 
        if (m_StartY == -1 || m_StartX == -1 || m_EndX == -1 || m_EndY == -1)
            return;
        for (int i = m_StartY; i <= m_EndY; ++i)
        {
            for (int j = m_StartX; j <= m_EndX; ++j)
            {
                m_vecTile[i * m_TileCountX + j]->PrevRender();
            }
        }
    }
}

void CTileMap::Render(HDC hDC)
{
    CMapBase::Render(hDC);
    if (!m_vecTile.empty())
    {
        // 혹시나 아직 세팅이 안됐다면 
        if (m_StartY == -1 || m_StartX == -1 ||
            m_EndX == -1 || m_EndY == -1) return;
        for (int i = m_StartY; i <= m_EndY; ++i)
        {
            for (int j = m_StartX; j <= m_EndX; ++j)
            {
                m_vecTile[i * m_TileCountX + j]->Render(hDC);
            }
        }
    }
}
