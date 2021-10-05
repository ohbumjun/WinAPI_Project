#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "GameManager.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR lpCmdLine,
					  _In_ int nCmdShow)
{
	// _CrtSetBreakAlloc(2096);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// Debug ��ġ
	//new char;

	srand((unsigned int)GetTickCount());
	rand();

	if (!CGameManager::GetInst()->Init(hInstance))
	{
		CGameManager::DestroyInst();
		return 0;
	}

	int RetValue = CGameManager::GetInst()->Run();

	CGameManager::DestroyInst();
	return RetValue;
}
